#include "Protocol.h"

void Protocol::processByte(int8_t byte)
{
    switch(awaiting_comm_byte) 
    {
        case CommFragmentType::Enquiry:
        {
            if (byte == CommFragmentType::Enquiry)
            {
                sendACK();
                awaiting_comm_byte = CommFragmentType::MessageByte;
                resetMessageBuffers();
            }
            break;
        }

        case CommFragmentType::MessageByte:
        {
          ParsedFrameResultType result = processFrameByte(byte);

          if (result == ParsedFrameResultType::Success)
          {
            sendACK();
            awaiting_comm_byte = CommFragmentType::EndOfTransmission;
          }
          else if (result == ParsedFrameResultType::Failure)
          {
            sendNACK();
            awaiting_comm_byte = CommFragmentType::MessageByte;
            resetMessageBuffers();
          }
          else if (result == ParsedFrameResultType::NotFinished)
          {
            // log here
          }
          break;
        }

        case CommFragmentType::EndOfTransmission:
        {
            if (byte == CommFragmentType::EndOfTransmission)
            {
                awaiting_comm_byte = CommFragmentType::Enquiry;
            }
            break;
        }
    }
}

void Protocol::processFrameByte(int8_t byte)
{
    switch(awaiting_frame_byte)
    {
        case FrameFragmentType::StartOfFrame:
        {

            break;
        }

        case FrameFragmentType::FrameLength:
        {

            break;
        }

        case FrameFragmentType::FrameByte:
        {

            break;
        }

        case FrameFragmentType::Checksum:
        {

            break;
        }
        
        case FrameFragmentType::EndOfFrame:
        {

            break;
        }


    }
}

void Protocol::resetMessageBuffers()
{
  current_frame_len    = 0;
  frame_bytes_received = 0;

  for (int i = 0; i < MAX_FRAME_BUFFER; ++i)
      frame_buffer[i] = 0;
  
  awaiting_frame_byte = FrameFragmentType::StartOfFrame;

}