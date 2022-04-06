


#ifndef __C_BM_VPU_H__
#define __C_BM_VPU_H__

#include <vector>
#include <iostream>
#include "decode/bmvpuapi_enc.h"
#include "decode/bmvpuapi_common.h"
#include <decode/bmqueue.h>
#include <cstdio>
#include <string>
#include <stdarg.h>
#include <cstdarg>
#include <mutex>


//////////////////////////////////////
//
//static std::string out_file_name = "./log/" + std::to_string(::time(NULL)) + ".log";
//static FILE* out_file_ptr = ::fopen(out_file_name.c_str(), "wb+");
//static std::mutex g_mutex;
//typedef std::lock_guard<std::mutex> clock_guard;
//
//static void var_log(const char* format, ...)
//{
//    
//    if (out_file_ptr)
//    {
//        clock_guard lock(g_mutex);
//        va_list ap;
//        va_start(ap, format);
//        static const uint32_t buffer_size = 1024 * 1024;
//        static char buffer[buffer_size] = { 0 };
//        size_t len = vsnprintf(static_cast<char*>(&buffer[0]), buffer_size, format, ap);
//        //g_log_ptr->append_var(level, format, ap);
//        ::fprintf(out_file_ptr, "%s", std::string(buffer, len).c_str());
//
//        ::fprintf(stdout, "%s", std::string(buffer, len).c_str());
//        ::fprintf(out_file_ptr, "\n");
//        ::fprintf(stdout, "\n");
//        ::fflush(out_file_ptr);
//        ::fflush(stdout);
//        va_end(ap);
//    }
//}
//#define DEBUG_LOG(format, ...)		var_log(format, ##__VA_ARGS__)
//#define DEBUG_EX_LOG(format, ...)		DEBUG_LOG("[%s][%d]" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
//




namespace chen {
    struct bs_buffer
    {
        std::shared_ptr<uint8_t> mem;
    };
    void bmvpu_setup_logging(void);
	class cbm_encoder 
	{
	public:
        explicit cbm_encoder();
		~cbm_encoder();



    public:
        // ±àÂë²ÎÊý
      bool init(int soc_idx, int width, int height);

      int encoder_image(uint8_t * y_ptr, uint8_t * u_ptr, uint8_t* v_ptr, int width, int height, std::vector<uint8_t> & frame_packet, BmVpuFrameType & frame_type);
	
      void destroy();
    private:
		cbm_encoder(const cbm_encoder&);
		cbm_encoder& operator =(const cbm_encoder&);

	private:

       // int     ret = 0;
        int                 m_soc_idx ; // id 
        int                 m_core_idx ;  // vpu enc
        bool                m_is_end;
        size_t              m_bs_buffer_size ;
        unsigned int        m_bs_buffer_alignment ;

       // int raw_size = 1920 * 1080 * 3 / 2;

        bm_queue_t*         m_frame_unused_queue ;


        BmVpuDMABufferAllocator*    m_dma_memory_allocator ;
        BmVpuDMABuffer*             m_bs_dma_buffer ;
        BmVpuEncoder*               m_video_encoder ;

        BmVpuFramebuffer*           m_rec_fb_list ;
        BmVpuDMABuffer**            m_rec_fb_dmabuffers;


        BmVpuFramebuffer*           m_src_fb_list ;
        BmVpuDMABuffer**            m_src_fb_dmabuffers;

        BmVpuEncOpenParams          m_eop;
        BmVpuEncInitialInfo         m_initial_info;
        BmVpuEncParams              m_enc_params;

        BmVpuRawFrame               m_input_frame;
        BmVpuEncodedFrame           m_output_frame;
        struct bs_buffer  bs_buffer_ptr;
        //bs_buffer_ptr.mem = NULL;
        BmVpuCodecFormat            m_codec_fmt ;
        BmVpuColorFormat            m_enc_color_format ;

	};
}

#endif // __C_BM_VPU_H__