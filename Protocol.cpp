#include "Protocol.h"

void Protocol::processByte(uint8_t rcv_byte)
{
  
    ParsedFrameResultType result = processFrameByte(rcv_byte);
    /*
    CommFragmentType rcv_byte_type = static_cast<CommFragmentType>(rcv_byte);

    if (awaiting_comm_byte == CommFragmentType::Enquiry)
    {
        if (rcv_byte_type == CommFragmentType::Enquiry)
        {
            sendACK();
            awaiting_comm_byte = CommFragmentType::MessageByte;
            resetMessageBuffers();
        }

    } 
    else if (awaiting_comm_byte == CommFragmentType::MessageByte)
    {
        ParsedFrameResultType result = processFrameByte(rcv_byte);
  
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
    }
    else if (awaiting_comm_byte == CommFragmentType::EndOfTransmission)
    {
        if (rcv_byte_type == CommFragmentType::EndOfTransmission)
        {
            awaiting_comm_byte = CommFragmentType::Enquiry;
        }
        else
        {
          // received garbage instead
        }
    }
    else
    {
        // log here
    }
    */

}

Protocol::ParsedFrameResultType Protocol::processFrameByte(uint8_t frame_byte)
{
    FrameFragmentType frame_byte_type = static_cast<FrameFragmentType>(frame_byte);
    
    #ifdef DEBUG
      Serial.print("Received: ");
      Serial.print(frame_byte, DEC);
      Serial.print(" -> ");
    #endif
    
    if (awaiting_frame_byte == FrameFragmentType::StartOfFrame && frame_byte_type == FrameFragmentType::StartOfFrame)
    {
      
      #ifdef DEBUG
        Serial.println("STX");
      #endif
    
      awaiting_frame_byte = FrameFragmentType::FrameLength;
    } 
    else if (awaiting_frame_byte == FrameFragmentType::FrameLength)
    {
      #ifdef DEBUG
        Serial.println("LEN");
      #endif
      if ( frame_byte > MAX_FRAME_SIZE ) {
        #ifdef DEBUG
          Serial.println("[E] Frame length bigger than maximum");
        #endif
        return ParsedFrameResultType::Failure;
      }
      current_frame_len = frame_byte;
      awaiting_frame_byte = FrameFragmentType::FrameByte;
    }
    else if (awaiting_frame_byte == FrameFragmentType::FrameByte)
    {      
      #ifdef DEBUG
        Serial.println("PAYLOAD");
      #endif
      
      frame_buffer[frame_bytes_received++] = frame_byte;
      if (frame_bytes_received == current_frame_len) {
        awaiting_frame_byte = FrameFragmentType::Checksum;
      }
    }
    else if (awaiting_frame_byte == FrameFragmentType::Checksum)
    {
      #ifdef DEBUG
        Serial.println("CHECKSUM");
      #endif
      if ( checkChecksum(frame_byte) == true )
      {
        awaiting_frame_byte = FrameFragmentType::EndOfFrame;
      }
      else
      {
        #ifdef DEBUG
          Serial.println("CHECKSUM ERROR!!!");
        #endif 
        ParsedFrameResultType::Failure;
      }
    }
    else if (awaiting_frame_byte == FrameFragmentType::EndOfFrame && frame_byte_type == FrameFragmentType::EndOfFrame)
    {
      #ifdef DEBUG
        Serial.println("ETX");
      #endif
      parseFrameAndSetAnimation();
      resetMessageBuffers();
      return ParsedFrameResultType::Success;
    }
    else
    {
       #ifdef DEBUG
         Serial.println("[E] SOMETHING ELSE");
       #endif
       
       ParsedFrameResultType::Failure;
    }

    return ParsedFrameResultType::NotFinished;
}

void Protocol::parseFrameAndSetAnimation()
{
  AnimationManager::AnimationType anim_type = static_cast<AnimationManager::AnimationType>(*((uint32_t*)(&frame_buffer[0])));
  uint32_t time = (*((uint32_t*)(&frame_buffer[4])));
  
  uint8_t col_1, col_2, col_3;
  
  col_1 = frame_buffer[8];
  col_2 = frame_buffer[9];
  col_3 = frame_buffer[10];
  
  uint32_t freq  = (*((uint32_t*)(&frame_buffer[12])));
  uint32_t seed  = (*((uint32_t*)(&frame_buffer[16])));
  uint32_t flags = (*((uint32_t*)(&frame_buffer[20])));
 
  Color color = Color(col_1, col_2, col_3);
 
  am.setAnimation(anim_type, time, color, freq, seed, flags);
 
  /*
  Serial.print("Parsed frame: ");
  Serial.print(" Time: ");
  Serial.print(time, DEC);
  Serial.print(" Color: ");
  Serial.print(col_1, HEX);
  Serial.print(col_2, HEX);
  Serial.print(col_3, HEX);
  Serial.print(" Freq: ");
  Serial.print(freq, DEC);
  Serial.print(" Seed: ");
  Serial.print(seed, DEC);
  Serial.print(" Type: ");
  Serial.print((int)anim_type, DEC);
  Serial.print(" Flags: ");
  Serial.print(flags, DEC);
  */
}

void Protocol::resetMessageBuffers()
{
  current_frame_len    = 0;
  frame_bytes_received = 0;

  for (int i = 0; i < MAX_FRAME_BUFFER; ++i)
      frame_buffer[i] = 0;
  
  awaiting_frame_byte = FrameFragmentType::StartOfFrame;
}

bool Protocol::checkChecksum(uint8_t checksum) {
  
  uint8_t calculated_checksum = 0x00;
  for (int i = 0; i < current_frame_len; ++i)
    calculated_checksum ^= frame_buffer[i];
    
  if ( checksum == calculated_checksum )
    return true;
  else 
    return false;
}
