#include "cbm_vpu.h"

#include "../clog.h"
namespace chen {

#ifndef MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#endif

#define BM_MAX_CTU_NUM                     0x4000      // CTU num for max resolution = 8192x8192/(64x64)
#define BM_MAX_SUB_CTU_NUM                 (MAX_CTU_NUM*4)
#define BM_MAX_MB_NUM                      0x40000     // MB num for max resolution = 8192x8192/(16x16)
#define BM_VPU_ALIGN16(_x)             (((_x)+0x0f)&~0x0f)
#define BM_VPU_ALIGN64(_x)             (((_x)+0x3f)&~0x3f)



#define BM_VPU_MS()                     std::chrono::steady_clock::time_point cur_time;  \
    std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();   \
                                        std::chrono::steady_clock::duration dur;         \
                                         std::chrono::milliseconds ms;                   \
                                            uint32_t elapse;





   

    static void logging_fn(BmVpuLogLevel level,
        char const* file, int const line, char const* fn,
        const char* format, ...)
    {
        va_list args;

        char const* lvlstr = "";
        switch (level) {
        case BM_VPU_LOG_LEVEL_ERROR:   lvlstr = "ERROR";   break;
        case BM_VPU_LOG_LEVEL_WARNING: lvlstr = "WARNING"; break;
        case BM_VPU_LOG_LEVEL_INFO:    lvlstr = "INFO";    break;
        case BM_VPU_LOG_LEVEL_DEBUG:   lvlstr = "DEBUG";   break;
        case BM_VPU_LOG_LEVEL_TRACE:   lvlstr = "TRACE";   break;
        case BM_VPU_LOG_LEVEL_LOG:     lvlstr = "LOG";     break;
        default: break;
        }
        using namespace chen;

        DEBUG_EX_LOG("%s:%d (%s)   %s: ", file, line, fn, lvlstr);

        va_start(args, format);
        DEBUG_LOG(format, args);
        va_end(args);

        fprintf(stderr, "\n");
    }

    void bmvpu_setup_logging(void)
    {
        BmVpuLogLevel level = BM_VPU_LOG_LEVEL_INFO;

        bmvpu_set_logging_threshold(level);

        bmvpu_set_logging_function(logging_fn);


    }
   

    static void* acquire_output_buffer(void* context, size_t size, void** acquired_handle)
    {
        struct bs_buffer* buffer = (struct bs_buffer*)(context);
        //if (buffer->mem)
        //{
        //    ::free(buffer->mem);

        //    buffer->mem = NULL;
        //}
        ////void* mem;
        //if (!buffer->mem)
        //{
        //    buffer->mem = ::malloc(size);/*(void*)(s->avpkt->data)*/;
        //    memset(buffer->mem, 0, size);
        //}
        buffer->mem.reset(new uint8_t[size]);
        //DEBUG_EX_LOG("[info][%s][%d][size = %u][mem = %p]\n", __FUNCTION__, __LINE__, size, buffer->mem.get());
        //bs_buffer_t* s = (bs_buffer_t*)(context);

        /*int ret;

        ret = ff_alloc_packet2(s->avctx, s->avpkt, size, size);
        if (ret < 0) {
            av_log(s->avctx, AV_LOG_ERROR, "Error! Failed ff_alloc_packet2()!\n");
            return NULL;
        }*/


        * acquired_handle = buffer->mem.get();
        return buffer->mem.get();
    }
    static void finish_output_buffer(void* context, void* acquired_handle)
    {
        //DEBUG_EX_LOG("[info][%s][%d][acquired_handle = %p]\n", __FUNCTION__, __LINE__, acquired_handle);
        ((void)(context));
    }



    static int bm_image_upload(BmVpuEncoder* video_encoder, uint64_t dst_data[4], int dst_linesizes[4],
        const uint8_t* src_data[4], const int src_linesizes[4], int width, int height)
    {
        // 因为是YUV420P 所以有三个分量 Y、U、V
        int planes_nb = 3;

        //int  comp[3][8] = {
        //         { 0, 1, 0, 0, 8, 0, 7, 1 },        /* Y */
        //         { 1, 1, 0, 0, 8, 0, 7, 1 },        /* U */
        //         { 2, 1, 0, 0, 8, 0, 7, 1 },        /* V */
        //};
        //for (int i = 0; i < 3; ++i)
        //{
        //    planes_nb = MAX(planes_nb, comp[i][0] + 1);
        //}
        for (int i = 0; i < planes_nb; ++i)
        {
            uint64_t dst = dst_data[i];
            ptrdiff_t dst_linesize = dst_linesizes[i];
            const uint8_t* src = src_data[i];
            ptrdiff_t src_linesize = src_linesizes[i];

            int h = height;

            ptrdiff_t bwidth = width;
            if (i != 0)
            {
                bwidth = ((width + 1)/2)/ 2;
            }
            //DEBUG_EX_LOG("[info]  bmvpu_upload_data[%d] ... \n", i);
            int ret = bmvpu_upload_data(video_encoder->core_idx, src, src_linesize,
                dst, dst_linesize, bwidth, h);
            if (ret < 0)
            {
                DEBUG_EX_LOG("[error] bmvpu_upload_data failed !!! (i = %d)\n", i);
                return -1;
            }
           // DEBUG_EX_LOG("[info]  bmvpu_upload_data[%d] end ok  ... \n", i);
        }
        return 0;

    }

	cbm_encoder::cbm_encoder()
		: m_soc_idx(-1)
        , m_core_idx(-1)
        , m_is_end(false)
        , m_bs_buffer_size(0)
        , m_bs_buffer_alignment(0)
        , m_frame_unused_queue(NULL)
        , m_dma_memory_allocator(NULL)
        , m_bs_dma_buffer(NULL)
        , m_video_encoder(NULL)
        , m_rec_fb_list(NULL)
        , m_rec_fb_dmabuffers(NULL)
        , m_src_fb_list(NULL)
        , m_src_fb_dmabuffers(NULL)
        , m_eop()
        , m_initial_info()
        , m_enc_params()
        , m_input_frame()
        , m_output_frame()
        , m_codec_fmt(BM_VPU_CODEC_FORMAT_H264)
        , m_enc_color_format(BM_VPU_COLOR_FORMAT_YUV420)
	{
        // log info 
        bmvpu_setup_logging();

	}
	cbm_encoder::~cbm_encoder()
	{
	}
    bool cbm_encoder::init(int soc_idx, int width, int height)
    {
        m_soc_idx = soc_idx;
        int ret = 0;
        DEBUG_EX_LOG("load vpu firmware ...\n");
        ret = bmvpu_enc_load(m_soc_idx);
        if (ret != BM_VPU_ENC_RETURN_CODE_OK)
        {
            DEBUG_EX_LOG("[error] bmvpu_enc_load soc_idx = %d failed !!!", m_soc_idx);
        }
        DEBUG_EX_LOG(" load vpu firmware ok !!!\n");


        m_core_idx = bmvpu_enc_get_core_idx(m_soc_idx);;
        DEBUG_EX_LOG("bm vpu enc get core idx = %d\n", m_core_idx);
        m_is_end = false;

        m_dma_memory_allocator = bmvpu_get_default_allocator();
        if (!m_dma_memory_allocator)
        {
            DEBUG_EX_LOG("[error]  alloc external DMA buffer  failed !!!\n");
            return -2;
        }
        DEBUG_EX_LOG("bmvpu_get_default_allocator alloc external DMA buffer ok !!!\n");

        //设置编码
        m_codec_fmt = BM_VPU_CODEC_FORMAT_H264;

        bmvpu_enc_set_default_open_params(&m_eop, m_codec_fmt);
        DEBUG_EX_LOG("enc set default open params H264 default set !!!");



        /////////////////////////////////////////////////////////////////////////////
        m_enc_color_format = BM_VPU_COLOR_FORMAT_YUV420;

        m_eop.soc_idx = soc_idx;
        m_eop.frame_width = width;
        m_eop.frame_height = height;
        m_eop.color_format = m_enc_color_format;
        m_eop.timebase_den = 50;
        m_eop.timebase_num = 1;
        m_eop.fps_num = 50;
        m_eop.fps_den = 1000; //

        // bitrate in kbps 
        m_eop.bitrate = 3000;
        m_eop.vbv_buffer_size = m_eop.bitrate * 2;
        m_eop.enc_mode = 2; // 编码时 的量化 速度

        m_eop.intra_period = 4; // gop size;
        // I-P-P-P-P-P
        m_eop.gop_preset = 6;

        // 是否开启 ROI 编码的时量化 重要地方进行量化 
        m_eop.roi_enable = 0;

        // set YUV420 或者 NV12 
        m_eop.chroma_interleave = 0;



        bmvpu_enc_get_bitstream_buffer_info(&m_bs_buffer_size, &m_bs_buffer_alignment);

        DEBUG_EX_LOG("bmvpu_enc_get_bitstream_buffer_info m_bs_buffer_size = %u, m_bs_buffer_alignment = %u", m_bs_buffer_size, m_bs_buffer_alignment);
#define BS_MASK (1024 * 4 -1)
        if (m_bs_buffer_size < static_cast<size_t>(width * height * 3 * 3 / 2))
        {
            m_bs_buffer_size = static_cast<size_t>(width * height * 3 * 3 / 2);
        }

        m_bs_buffer_size = (m_bs_buffer_size + BS_MASK) & (~BS_MASK);
        m_bs_dma_buffer = bmvpu_dma_buffer_allocate(m_dma_memory_allocator, soc_idx, m_bs_buffer_size, m_bs_buffer_alignment, BM_VPU_ALLOCATION_FLAG_DEFAULT);
        if (!m_bs_dma_buffer)
        {
            DEBUG_EX_LOG("[error] bmvpu_dma_buffer_allocate dma buffer alloc failed !!!\n");
            return false;
        }

        // open an encoder instance 
        DEBUG_EX_LOG("open encoder ... \n");

        ret = bmvpu_enc_open(&m_video_encoder, &m_eop, m_bs_dma_buffer);

        if (ret != BM_VPU_ENC_RETURN_CODE_OK)
        {
            DEBUG_EX_LOG("bmvpu_enc_open failed !!!(ret = %d)\n", ret);
            return false;
        }


        if (!m_video_encoder)
        {
            DEBUG_EX_LOG("m_video_encoder alloc failed !!! \n");
            return false;
        }

        ret = bmvpu_enc_get_initial_info(m_video_encoder, &m_initial_info);
        if (ret != 0)
        {
            DEBUG_EX_LOG("[error] bmvpu_enc_get_initial_info failed !!!\n");
            return  false;
        }

        DEBUG_EX_LOG("num framebuffers for recon: %u\n", m_initial_info.min_num_rec_fb);


        m_rec_fb_list = static_cast<BmVpuFramebuffer*>(::malloc(sizeof(BmVpuFramebuffer) * m_initial_info.min_num_rec_fb));

        if (!m_rec_fb_list)
        {
            DEBUG_EX_LOG("alloc bmVpuFramebuffer failed !!!\n");
            return false;
        }


        m_rec_fb_dmabuffers = static_cast<BmVpuDMABuffer**>(::malloc(sizeof(BmVpuDMABuffer*) * m_initial_info.min_num_rec_fb));
        if (!m_rec_fb_dmabuffers)
        {
            DEBUG_EX_LOG("[error] alloc BmVpuDMABuffer** failed !!!\n");
            return false;
        }
        for (int i = 0; i < m_initial_info.min_num_rec_fb; ++i)
        {
            int rec_id = 0x200 + i;


            /**
             alloc a DMA buffer for each framebuffer
             it is possible to specify alternate allocators
            */
            m_rec_fb_dmabuffers[i] = bmvpu_dma_buffer_allocate(m_dma_memory_allocator,
                soc_idx, m_initial_info.rec_fb.size, m_initial_info.framebuffer_alignment,
                BM_VPU_ALLOCATION_FLAG_DEFAULT);

            bmvpu_fill_framebuffer_params(&m_rec_fb_list[i], &m_initial_info.rec_fb,
                m_rec_fb_dmabuffers[i], rec_id, NULL);

        }


        // registeration help the VPU  
        ret = bmvpu_enc_register_framebuffers(m_video_encoder, m_rec_fb_list, m_initial_info.min_num_rec_fb);

        if (ret != 0)
        {
            DEBUG_EX_LOG("[error]bmvpu_enc_register_framebuffers failed !!! \n ");
            return -10;
        }

        DEBUG_EX_LOG("[info] num framebuffers for src: %u\n", m_initial_info.min_num_src_fb);

        m_src_fb_list = static_cast<BmVpuFramebuffer*>(::malloc(sizeof(BmVpuFramebuffer) * m_initial_info.min_num_src_fb));


        if (!m_src_fb_list)
        {
            DEBUG_EX_LOG("[error] alloc BmVpuFramebuffer* failed !!!\n");
            return -11;
        }



        m_src_fb_dmabuffers = static_cast<BmVpuDMABuffer**> (::malloc(sizeof(BmVpuDMABuffer*) * m_initial_info.min_num_src_fb));
        if (!m_src_fb_dmabuffers)
        {
            DEBUG_EX_LOG("[error] BmVpuDMABuffer * * alloc failed !!!\n");
            return -12;
        }
        for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
        {
            int src_id = 0x100 + i;
            // alloc a DMA buffer for each raw input frame 
            m_src_fb_dmabuffers[i] = bmvpu_dma_buffer_allocate(m_dma_memory_allocator,
                soc_idx, m_initial_info.src_fb.size, m_initial_info.framebuffer_alignment,
                BM_VPU_ALLOCATION_FLAG_DEFAULT);

            bmvpu_fill_framebuffer_params(&m_src_fb_list[i], &m_initial_info.src_fb,
                m_src_fb_dmabuffers[i], src_id, NULL);
        }
        memset(&m_enc_params, 0, sizeof(BmVpuEncParams));


        m_enc_params.roi_dma_buffer = static_cast<BmVpuDMABuffer**>(::malloc(sizeof(BmVpuDMABuffer*) * m_initial_info.min_num_src_fb)); // 编码量化

        if (!m_enc_params.roi_dma_buffer)
        {
            DEBUG_EX_LOG("[error] enc_params.roi_dma_buffer ** alloc failed !!!\n");
            return -12;
        }
        DEBUG_EX_LOG("[info] roi_dma_buffer alloc ok !!!\n");



        m_enc_params.customMapOpt = static_cast<BmCustomMapOpt*>(::malloc(sizeof(BmCustomMapOpt) * m_initial_info.min_num_src_fb));
        if (!m_enc_params.customMapOpt)
        {
            DEBUG_EX_LOG("[error] enc_params.customMapOpt alloc failed !!!\n");
            return -13;
        }

        DEBUG_EX_LOG("[info]enc_params.customMapOpt alloc ok !!!\n ");


        m_enc_params.customMapOptUsedIndex = 0;

        for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
        {
            int alignment = 1;
            m_enc_params.roi_dma_buffer[i] = bmvpu_dma_buffer_allocate(m_dma_memory_allocator, soc_idx,
                BM_MAX_MB_NUM, alignment, BM_VPU_ALLOCATION_FLAG_DEFAULT);

            m_enc_params.customMapOpt[i].addrCustomMap = bmvpu_dma_buffer_get_physical_address(m_enc_params.roi_dma_buffer[i]);
        }


        // create queue for source frame unused 
        m_frame_unused_queue = bm_queue_create(m_initial_info.min_num_src_fb, sizeof(BmVpuFramebuffer*));
        if (!m_frame_unused_queue)
        {
            DEBUG_EX_LOG("[error] create queue failed !!!\n");
            return -14;
        }
        DEBUG_EX_LOG("[info] create queue ok !!!\n");

        for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
        {
            BmVpuFramebuffer* fb = &m_src_fb_list[i];
            bm_queue_push(m_frame_unused_queue, (void*)(&fb));
            DEBUG_EX_LOG("[info] myindex = 0x%x, %p, push\n", fb->myIndex, fb);
        }

        // log mem addres info 
       // bm_queue_show(frame_unused_queue);


        /* Create avframe list for source frame */
       /* ctx->avframe_used_list = av_mallocz(ctx->num_src_fb * sizeof(AVFrame*));
        if (ctx->avframe_used_list == NULL) {
            av_log(avctx, AV_LOG_ERROR, "bm_queue_create failed\n");
            ret = AVERROR_UNKNOWN;
            goto cleanup;
        }*/


        /* Set up the encoding parameters */
        m_enc_params.acquire_output_buffer = acquire_output_buffer; //  acquire_output_buffer;
        m_enc_params.finish_output_buffer = finish_output_buffer; // finish_output_buffer;
        m_enc_params.output_buffer_context = NULL;

        memset(&m_input_frame, 0, sizeof(m_input_frame));
        memset(&m_output_frame, 0, sizeof(m_output_frame));

        DEBUG_EX_LOG("[info] bm enc init  ok !!!\n");


        return true;
    }
    int cbm_encoder::encoder_image( uint8_t* y_ptr,   uint8_t* u_ptr,   uint8_t* v_ptr, int width, int height, std::vector<uint8_t>& frame_packet, BmVpuFrameType& frame_type)
    {
        /*int width = 1920;
        int height = 1080;*/
        int ret = 0;

        /* The input frames come in Non-DMA memory */
        uint8_t* addr;
        uint8_t* src_y, * src_u, * src_v;
        uint8_t* dst_y, * dst_u, * dst_v;
        int src_stride_y, src_stride_u, src_stride_v;
        int dst_stride_y, dst_stride_u, dst_stride_v;

        BmVpuFramebuffer* src_fb = NULL;
        {
            BmVpuFramebuffer* fb = *((BmVpuFramebuffer**)bm_queue_pop(m_frame_unused_queue));

            if (!fb)
            {
                DEBUG_EX_LOG("[error] frame buffer is NULL, pop failed\n");
                return -15;
            }
           // DEBUG_EX_LOG("[info] myIndex = 0x%x, %p, pop \n", fb->myIndex, fb);
            for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
            {
                if (&m_src_fb_list[i] == fb)
                {
                    src_fb = fb;
                   // DEBUG_EX_LOG("[info] find frame buffer i = %d\n", i);
                    break;
                }
            }
        }
        if (!src_fb)
        {
            DEBUG_EX_LOG("[error]bm_queue_pop not find src_fd failed !!!");
            return -16;
        }
        //DEBUG_EX_LOG("[info]bmvpu_dma_buffer_map alloc ...\n");


        BM_VPU_MS();

        addr = (uint8_t*)bmvpu_dma_buffer_get_physical_address(src_fb->dma_buffer);
        if (!addr) 
        {
            DEBUG_EX_LOG("[error]bmvpu_dma_buffer_get_physical_address failed\n");
            return -16;
        }
        cur_time = std::chrono::steady_clock::now();

        dur = cur_time - pre_time;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
         elapse = static_cast<uint32_t>(ms.count());
        DEBUG_EX_LOG("bmvpu_dma_buffer_get_physical_address ms = %u", elapse);
        pre_time = cur_time;
       // DEBUG_EX_LOG("[info]bmvpu_dma_buffer_map alloc ok !!! \n");



        dst_y = addr + src_fb->y_offset;
        dst_u = addr + src_fb->cb_offset;
        dst_v = addr + src_fb->cr_offset;
        dst_stride_y = src_fb->y_stride;
        dst_stride_u = src_fb->cbcr_stride;
        dst_stride_v = src_fb->cbcr_stride;



        /* Copy YUV420P to YUV420P */
     // uint8_t* data[8];
     // uint8_t linesize[8];
        src_y = y_ptr;// data[0];
        src_u = u_ptr;// data[1];
        src_v = v_ptr;// data[2];
        src_stride_y = width; // linesize[0];
        src_stride_u = (width +1) / 2; // linesize[1];
        src_stride_v = (width+1 ) / 2; // linesize[2];


       // DEBUG_EX_LOG("[info][%s] copying ...\n", __FUNCTION__);
        {
            const uint8_t* src_data[4] = { src_y, src_u, src_v, NULL };
            const int src_linesizes[4] = { src_stride_y, src_stride_u, src_stride_v, 0 };
            int dst_linesizes[4] = { dst_stride_y, dst_stride_u, dst_stride_v, 0 };


            uint64_t dst_data[4] = { (uint64_t)dst_y, (uint64_t)dst_u, (uint64_t)dst_v, 0L };
         //   DEBUG_EX_LOG("[info] bm_image_upload ...\n");
            ret = bm_image_upload(m_video_encoder,
                dst_data, dst_linesizes,
                src_data, src_linesizes,
                width, height);
            if (ret < 0)
            {
                DEBUG_EX_LOG("[error]bm_image_upload failed\n");
                return -20;
            }
         //   DEBUG_EX_LOG("[info] bm_image_upload ok !!!\n");

            /*uint8_t* dst_data[4] = { dst_y, dst_u, dst_v, NULL };


            memcpy(dst_y, src_y, width * height);
            memcpy(dst_u, src_u, width * height / 4);
            memcpy(dst_v, src_v, width * height / 4);*/
        }
        cur_time = std::chrono::steady_clock::now();

        dur = cur_time - pre_time;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
         elapse = static_cast<uint32_t>(ms.count());
        DEBUG_EX_LOG("bm_image_upload ms = %u", elapse);
        pre_time = cur_time;
       // DEBUG_EX_LOG("[info][%s] copying .. Done\n", __FUNCTION__);

        /* Flush cache to DMA buffer */
        bmvpu_dma_buffer_flush(src_fb->dma_buffer);

        /* Unmap the DMA buffer of the raw frame */
        bmvpu_dma_buffer_unmap(src_fb->dma_buffer);


        m_enc_params.skip_frame = 0;
        m_input_frame.framebuffer = src_fb;
        m_input_frame.context = NULL;
       // DEBUG_EX_LOG("[info] set input frame ok !!!\n");



        m_enc_params.output_buffer_context = (void*)(&(bs_buffer_ptr));
        // enc_params.output_buffer_context = bs_buffer;
        BmVpuEncReturnCodes enc_ret;
        unsigned int output_code = 0; // TODO

        m_output_frame.data_size = 0;

        cur_time = std::chrono::steady_clock::now();

        dur = cur_time - pre_time;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
        elapse = static_cast<uint32_t>(ms.count());
        DEBUG_EX_LOG("bmvpu_dma_buffer_unmap ms = %u", elapse);
        pre_time = cur_time;


        enc_ret = static_cast<BmVpuEncReturnCodes>(bmvpu_enc_encode(m_video_encoder, &m_input_frame,
            &m_output_frame, &m_enc_params, &output_code));
        if (enc_ret == BM_VPU_ENC_RETURN_CODE_END)
        {
            DEBUG_EX_LOG("encoding end !!!\n");
            m_is_end = true;
        }
        else if (enc_ret != BM_VPU_ENC_RETURN_CODE_OK)
        {
            DEBUG_EX_LOG("[error] could not encode this image : %s\n", bmvpu_enc_error_string(enc_ret));
            return -17;
        }
        cur_time = std::chrono::steady_clock::now();

        dur = cur_time - pre_time;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
        elapse = static_cast<uint32_t>(ms.count());
        DEBUG_EX_LOG("bmvpu_enc_encode ms = %u", elapse);
        pre_time = cur_time;
        if (m_output_frame.data_size > 0)
        {
            frame_type = m_output_frame.frame_type;
           // static int count = 0;
           // std::string out_h264_file_name = "./hex/h264_" + std::to_string(++count) + ".hex";
            //  FILE* out_hex_ptr = ::fopen(out_h264_file_name.c_str(), "wb+");
           // DEBUG_EX_LOG("[info] VPU FrameType = %d, [data_size = %u]\n", output_frame.frame_type, output_frame.data_size);
            /////* std::string hex = hexdump(output_frame.data, output_frame.data_size);
            //// ::fwrite(hex.c_str(), 1, hex.length(), out_hex_ptr);*/
            // ::fwrite(bs_buffer_ptr.mem, 1, output_frame.data_size, out_hex_ptr);
            // //::fwrite(hex.c_str(), 1, hex.length(), out_hex_ptr);
            // ::fprintf(out_hex_ptr, "\n=============count = %d=====================\n", count -1);
            // ::fflush(out_hex_ptr);
            // ::fclose(out_hex_ptr);
            // out_hex_ptr = NULL;
             /* Collect the input frames released */
            for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
            {
                BmVpuFramebuffer* fb = &(m_src_fb_list[i]);
                if (fb->myIndex != m_output_frame.src_idx)
                {
                    continue;
                }
                bm_queue_push(m_frame_unused_queue, &fb);
              //  DEBUG_EX_LOG("[info] Myindex = 0x%x, push\n", fb->myIndex);
                break;
            }

            frame_packet.resize(m_output_frame.data_size);
        //    DEBUG_EX_LOG("encoder frame data size = %u", m_output_frame.data_size);
            ::memcpy(&frame_packet[0], bs_buffer_ptr.mem.get(), m_output_frame.data_size);
            cur_time = std::chrono::steady_clock::now();

            dur = cur_time - pre_time;
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
            elapse = static_cast<uint32_t>(ms.count());
            DEBUG_EX_LOG("nal copy  ms = %u", elapse);
            pre_time = cur_time;
        }
       // DEBUG_EX_LOG("encoder frame end ok \n");

        return 0;
    }
    void cbm_encoder::destroy()
    {
        DEBUG_EX_LOG("encoder destroy !!!");
        if (m_video_encoder)
        {
            bmvpu_enc_close(m_video_encoder);
            m_video_encoder = NULL;
            DEBUG_EX_LOG("video encoder close ok !!!");
        }

        if (m_rec_fb_list)
        {
            ::free(m_rec_fb_list);
            m_rec_fb_list = NULL;
            DEBUG_EX_LOG("rec fb list free ok !!!");
        }

        if (m_rec_fb_dmabuffers)
        {
            for (int i = 0; i < m_initial_info.min_num_rec_fb; ++i)
            {
                bmvpu_dma_buffer_deallocate(m_rec_fb_dmabuffers[i]);
            }
            ::free(m_rec_fb_dmabuffers);
            m_rec_fb_dmabuffers = NULL;
            DEBUG_EX_LOG("rec fb dmabuffers free ok !!!");
        }
        if (m_frame_unused_queue)
        {
            bm_queue_destroy(m_frame_unused_queue);
            m_frame_unused_queue = NULL;
            DEBUG_EX_LOG("frame unused queue free ok !!!");
        }

        if (m_src_fb_dmabuffers)
        {
            for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
            {
                bmvpu_dma_buffer_deallocate(m_src_fb_dmabuffers[i]);
            }
            ::free(m_src_fb_dmabuffers);
            m_src_fb_dmabuffers = NULL;
            DEBUG_EX_LOG("src fb dma buffer free ok !!!");
        }
        if (m_src_fb_list)
        {
            ::free(m_src_fb_list);
            m_src_fb_list = NULL;
            DEBUG_EX_LOG("src_fb_list free ok !!!");
        }

        if (m_bs_dma_buffer)
        {
            bmvpu_dma_buffer_deallocate(m_bs_dma_buffer);
            m_bs_dma_buffer = NULL;
            DEBUG_EX_LOG("bs dma buffer deallocate ok !!!");
        }

        if (m_enc_params.roi_dma_buffer)
        {
            for (int i = 0; i < m_initial_info.min_num_src_fb; ++i)
            {
                bmvpu_dma_buffer_deallocate(m_enc_params.roi_dma_buffer[i]);
                m_enc_params.roi_dma_buffer[i] = NULL;
            }
            ::free(m_enc_params.roi_dma_buffer);
            m_enc_params.roi_dma_buffer = NULL;
        }
        // unload the VPU firmware 
        int ret = bmvpu_enc_unload(m_soc_idx);
        if (ret != BM_VPU_ENC_RETURN_CODE_OK)
        {
            DEBUG_EX_LOG("Failed to call bmvpu_enc_unload !!!");
        }
        else
        {
            DEBUG_EX_LOG("bmvpu_enc_unload ok !!!");
        }


    }
}