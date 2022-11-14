/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_MEDIASOUP_INPUT_DEVICE_EVENT_H
#define C_MEDIASOUP_INPUT_DEVICE_EVENT_H
#include <thread>
#include <string>
#include <functional>
namespace cmediasoup
{
	namespace EMouseButtons
	{
		enum Type
		{
			Left = 0,
			Middle,
			Right,
			Thumb01,
			Thumb02,

			Invalid,
		};
	}





	/** The types of event which can be processed by the device. */
	enum class EventType
	{
		UNDEFINED,		 /** No value. */
		KEY_DOWN,		 /** A key has been pushed down. */
		KEY_UP,			 /** A key has been released. */
		KEY_PRESS,		 /** A key has been pressed and a character has been input. */
		MOUSE_ENTER,	 /** The mouse has entered canvas. */
		MOUSE_LEAVE,	 /** The mouse has left the canvas. */
		MOUSE_MOVE,		 /** The mouse has been moved. */
		MOUSE_DOWN,		 /** A mouse button has been clicked. */
		MOUSE_UP,		 /** A mouse button has been released. */
		MOUSE_WHEEL,	 /** The mouse wheel was scrolled. */
		TOUCH_START,	 /** A finger is put down onto the canvas. */
		TOUCH_END,		 /** A finger is lifted from the canvas. */
		TOUCH_MOVE,		 /** A finger is being dragged along the surface of the canvas. */
		GAMEPAD_PRESS,	 /** A gamepad button has been pressed. */
		GAMEPAD_RELEASE, /** A gamepad button has been released. */
		GAMEPAD_ANALOG	 /** A gamepad analog stick has been moved. */
	};

	/** A general input event. */
	struct MEvent
	{
		/** The type of the general event. */
		EventType Event;

		/** A generic piece of data which is used to hold information about the
		* event, specialized by making a union with an appropriate struct. */
		union
		{
			uint64_t Word;

			struct   /** KEY_DOWN */
			{
				uint8_t KeyCode;
				bool bIsRepeat;
			} KeyDown;

			struct   /* KEY_UP */
			{
				uint8_t KeyCode;
			} KeyUp;

			struct   /** KEY_PRESSED */
			{
				char Character;
			} Character;

			struct   /** MOUSE_MOVE */
			{
				int16_t DeltaX;
				int16_t DeltaY;
				uint16_t PosX;
				uint16_t PosY;
			} MouseMove;

			struct   /** MOUSE_DOWN, MOUSE_UP */
			{
				uint8_t Button;
				uint16_t PosX;
				uint16_t PosY;
			} MouseButton;

			struct   /** MOUSE_WHEEL */
			{
				int16_t Delta;
				uint16_t PosX;
				uint16_t PosY;
			} MouseWheel;

			struct   /** TOUCH_START, TOUCH_END, TOUCH_MOVE */
			{
				uint8_t TouchIndex;
				uint16_t PosX;
				uint16_t PosY;
				uint8_t Force;
			} Touch;

			struct   /** GAMEPAD_PRESSED, GAMEPAD_RELEASED */
			{
				uint8_t ControllerIndex;
				uint8_t ButtonIndex;
				bool bIsRepeat;

			} GamepadButton;

			struct   /** GAMEPAD_PRESSED, GAMEPAD_RELEASED */
			{
				uint8_t ControllerIndex;
				float AnalogValue;
				uint8_t AxisIndex;
			} GamepadAnalog;
		} Data;

		/**
		* Create a completely empty event.
		*/
		MEvent()
			: Event(EventType::UNDEFINED)
		{
		}

		/**
		* Create an event of the given type.
		* @param InEvent - The type of the event.
		*/
		MEvent(EventType InEvent)
			: Event(InEvent)
		{
		}

		/**
		* An event related to gamepad analog stick being moved.
		* @param InControllerIndex - Number representing the index of the controller.
		* @param InAxisIndex - The axis that is non-centered.
		* @param InAnalogValue - The value of the analog stick.
		*/
		void SetGamepadAnalog(uint8_t InControllerIndex, uint8_t InAxisIndex, float InAnalogValue)
		{
			 
			Data.GamepadAnalog.ControllerIndex = InControllerIndex;
			Data.GamepadAnalog.AxisIndex = InAxisIndex;
			Data.GamepadAnalog.AnalogValue = InAnalogValue;
		}

		/**
		* An event related to a gamepad button being pushed down.
		* @param InControllerIndex - Number representing the index of the controller.
		* @param InButtonIndex - Numerical code identifying the pushed down button.
		* @param InIsRepeat - Whether the button is being kept down and is repeating.
		*/
		void SetGamepadButtonPressed(uint8_t InControllerIndex, uint8_t InButtonIndex, bool InIsRepeat)
		{
			 
			Data.GamepadButton.ControllerIndex = InControllerIndex;
			Data.GamepadButton.ButtonIndex = InButtonIndex;
			Data.GamepadButton.bIsRepeat = InIsRepeat;
		}

		/**
		* An event related to a gamepad button being released.
		* @param InControllerIndex - Number representing the index of the controller.
		* @param InButtonIndex - Numerical code identifying the released button.
		*/
		void SetGamepadButtonReleased(uint8_t InControllerIndex, uint8_t InButtonIndex)
		{
			 
			Data.GamepadButton.ControllerIndex = InControllerIndex;
			Data.GamepadButton.ButtonIndex = InButtonIndex;
		}

		/**
		* An event related to a key being pushed down.
		* @param InKeyCode - Numerical code identifying the pushed down key.
		* @param InIsRepeat - Whether the key is being kept down and is repeating.
		*/
		void SetKeyDown(uint8_t InKeyCode, bool InIsRepeat)
		{
			 
			Data.KeyDown.KeyCode = InKeyCode;
			Data.KeyDown.bIsRepeat = InIsRepeat;
		}

		/**
		* An event related to a key being released.
		* @param InKeyCode - Numerical code identifying the released key.
		*/
		void SetKeyUp(uint8_t InKeyCode)
		{
			 
			Data.KeyUp.KeyCode = InKeyCode;
		}

		/**
		* An event related to character input.
		* @param InCharacter - The character being input.
		*/
		void SetCharCode(char InCharacter)
		{
			 
			Data.Character.Character = InCharacter;
		}

		/**
		* An event related to mouse movement.
		* @param InPoxX - The X position of the mouse pointer.
		* @param InPosY - The Y position of the mouse pointer.
		* @param InDeltaX - The change in the X position of the mouse pointer.
		* @param InDeltaY - The change in the Y position of the mouse pointer.
		*/
		void SetMouseDelta(uint16_t InPosX, uint16_t InPosY, int16_t InDeltaX, int16_t InDeltaY)
		{
			 
			Data.MouseMove.DeltaX = InDeltaX;
			Data.MouseMove.DeltaY = InDeltaY;
			Data.MouseMove.PosX = InPosX;
			Data.MouseMove.PosY = InPosY;
		}

		/**
		* An event related to mouse buttons.
		* @param InButton - The button number corresponding to left, middle, right, etc.
		* @param InPoxX - The X position of the mouse pointer.
		* @param InPosY - The Y position of the mouse pointer.
		*/
		void SetMouseClick(uint8_t InButton, uint16_t InPosX, uint16_t InPosY)
		{  
			Data.MouseButton.Button = InButton;
			Data.MouseButton.PosX = InPosX;
			Data.MouseButton.PosY = InPosY;
		}

		/**
		* An event related to the mouse scroll wheel.
		* @param InButton - The amount by which the mouse wheel was scrolled.
		* @param InPoxX - The X position of the mouse pointer when the wheel was scrolled.
		* @param InPosY - The Y position of the mouse pointer when the wheel was scrolled.
		*/
		void SetMouseWheel(int16_t InDelta, uint16_t InPosX, uint16_t InPosY)
		{
			 
			Data.MouseWheel.Delta = InDelta;
			Data.MouseWheel.PosX = InPosX;
			Data.MouseWheel.PosY = InPosY;
		}

		/**
		* An event related to a finger touching the canvas.
		* @param InTouchIndex - The finger used in multi-touch.
		* @param InPoxX - The X position of the finger.
		* @param InPosY - The Y position of the finger.
		* @param InForce - The amount of pressure being applied by the finger.
		*/
		void SetTouch(uint8_t InTouchIndex, uint16_t InPosX, uint16_t InPosY, uint8_t InForce)
		{
			 Data.Touch.TouchIndex = InTouchIndex;
			Data.Touch.PosX = InPosX;
			Data.Touch.PosY = InPosY;
			Data.Touch.Force = InForce;
		}

		/**
		* Get information about an event related to gamepad analog stick being moved.
		* @param OutControllerIndex - Number representing the index of the controller.
		* @param OutAxisIndex - Number representing the axis of the controller.
		* @param OutAnalogValue - The value of the analog stick.
		*/
		void GetGamepadAnalog(uint8_t& OutControllerIndex, uint8_t& OutAxisIndex, float& OutAnalogValue)
		{
			 
			OutControllerIndex = Data.GamepadAnalog.ControllerIndex;
			OutAxisIndex = Data.GamepadAnalog.AxisIndex;
			OutAnalogValue = Data.GamepadAnalog.AnalogValue;
		}

		/**
		* Get information about an event related to a gamepad button being pushed down.
		* @param OutControllerIndex - Number representing the index of the controller.
		* @param OutButtonIndex - Numerical code identifying the pushed down button.
		* @param OutIsRepeat - Whether the button is being kept down and is repeating.
		*/
		void GetGamepadButtonPressed(uint8_t& OutControllerIndex, uint8_t& OutButtonIndex, bool& OutIsRepeat)
		{
			 
			OutControllerIndex = Data.GamepadButton.ControllerIndex;
			OutButtonIndex = Data.GamepadButton.ButtonIndex;
			OutIsRepeat = Data.GamepadButton.bIsRepeat;
		}

		/**
		* Get information about an event related to a gamepad button being released.
		* @param OutControllerIndex - Number representing the index of the controller.
		* @param OutButtonIndex - Numerical code identifying the pushed down button.
		*/
		void GetGamepadButtonReleased(uint8_t& OutControllerIndex, uint8_t& OutButtonIndex)
		{
			 
			OutControllerIndex = Data.GamepadButton.ControllerIndex;
			OutButtonIndex = Data.GamepadButton.ButtonIndex;
		}

		/**
		* Get information about an event related to a key being pushed down.
		* @param OutKeyCode - Numerical code identifying the pushed down key.
		* @param OutIsRepeat - Whether the key is being kept down and is repeating.
		*/
		void GetKeyDown(uint8_t& OutKeyCode, bool& OutIsRepeat)
		{
			 
			OutKeyCode = Data.KeyDown.KeyCode;
			OutIsRepeat = Data.KeyDown.bIsRepeat;
		}

		/**
		* Get information about an event related to a key being released.
		* @param OutKeyCode - Numerical code identifying the released key.
		*/
		void GetKeyUp(uint8_t& OutKeyCode)
		{
			 
			OutKeyCode = Data.KeyUp.KeyCode;
		}

		/**
		* Get information about an event related to character input.
		* @param OutCharacter - The character being input.
		*/
		void GetCharacterCode(char& OutCharacter)
		{
			 
			OutCharacter = Data.Character.Character;
		}

		/**
		* Get information about an event related to mouse movement.
		* @param OutPoxX - The X position of the mouse pointer.
		* @param OutPosY - The Y position of the mouse pointer.
		* @param OutDeltaX - The change in the X position of the mouse pointer.
		* @param OutDeltaY - The change in the Y position of the mouse pointer.
		*/
		void GetMouseDelta(uint16_t& OutPosX, uint16_t& OutPosY, int16_t& OutDeltaX, int16_t& OutDeltaY)
		{
			 
			OutPosX = Data.MouseMove.PosX;
			OutPosY = Data.MouseMove.PosY;
			OutDeltaX = Data.MouseMove.DeltaX;
			OutDeltaY = Data.MouseMove.DeltaY;
		}

		/**
		* Get information about an event related to mouse buttons.
		* @param OutButton - The button number corresponding to left, middle, right, etc.
		* @param OutPosX - The X position of the mouse pointer.
		* @param OutPosY - The Y position of the mouse pointer.
		*/
		void GetMouseClick(EMouseButtons::Type& OutButton, uint16_t& OutPosX, uint16_t& OutPosY)
		{
			//check(Event == EventType::MOUSE_DOWN || Event == EventType::MOUSE_UP);
			// https://developer.mozilla.org/en-US/docs/Web/Events/mousedown
			uint8_t Button = Data.MouseButton.Button;
#if defined(_MSC_VER)
			switch (Button)
			{
			case 0:
			{
				OutButton = EMouseButtons::Left; // EMouseButtons::Left;
			}
			break;
			case 1:
			{
				OutButton = EMouseButtons::Middle; /*== EventType::MOUSE_DOWN ? WM_MBUTTONDOWN : WM_MBUTTONUP*/; ; // EMouseButtons::Middle;
			}
			break;
			case 2:
			{
				OutButton = EMouseButtons::Right; // EMouseButtons::Right;
			}
			break;
			case 3:
			{
				//log error ---> 
				//OutButton = EMouseButtons::Thumb01;
			}
			break;
			case 4:
			{
				//log error ---> 
			//	OutButton = EMouseButtons::Thumb02;
			}
			break;
			default:
			{
				// log error ---> 
				//UE_LOG(PixelStreamerInputDevice, Error, TEXT("Unknown Pixel Streaming mouse click with button %d and word 0x%016llx"), Button, Data.Word);
			}
			break;
			}
#endif //#if defined(_MSC_VER)
			OutPosX = Data.MouseButton.PosX;
			OutPosY = Data.MouseButton.PosY;
		}

		/**
		* Get information about an event related to the mouse wheel.
		* @param OutDelta - The amount by which the mouse wheel was scrolled.
		* @param PosX - The X position of the mouse pointer when the wheel was scrolled.
		* @param PosY - The Y position of the mouse pointer when the wheel was scrolled.
		*/
		void GetMouseWheel(int16_t& OutDelta, uint16_t& OutPosX, uint16_t& OutPosY)
		{
			 
			OutDelta = Data.MouseWheel.Delta;
			OutPosX = Data.MouseWheel.PosX;
			OutPosY = Data.MouseWheel.PosY;
		}

		 
	};
}

#endif // C_MEDIASOUP_INPUT_DEVICE_EVENT_H