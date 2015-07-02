#ifndef PROTOCOL_HEADER_GUARD
#define PROTOCOL_HEADER_GUARD

#define DEBUG

#include <Arduino.h>
#include "AnimationManager.h"

#define MAX_FRAME_BUFFER 128
#define MAX_MSG_BUFFER   128
#define MAX_FRAME_SIZE   100

enum ASCIICodes { 
  NUL = 0, 
  STX = 2, 
  ETX = 3,
  EOT = 4,
  ENQ = 5,
  ACK = 6,
  NACK = 21
};

enum class AnimationFlag {
  HSV = (1<<0),
  OtherFlag = (1<<1)
};


bool checkFlag(AnimationFlag flag, int flags);

class AnimationManager;

class Protocol 
{
  
  enum class CommFragmentType : uint8_t
  {
    Enquiry = ENQ, 
    MessageByte = NUL,
    EndOfTransmission = EOT
  };
  
  enum class FrameFragmentType : uint8_t
  {
    None = NUL,
    StartOfFrame = STX,
    EndOfFrame = ETX,
    FrameLength, 
    FrameByte, 
    Checksum
  };
  
  enum class ParsedFrameResultType
  {
     Success, NotFinished, Failure
  };

public:
  Protocol(AnimationManager& am): am(am), current_frame_len(0), frame_bytes_received(0), 
        awaiting_comm_byte(CommFragmentType::Enquiry), awaiting_frame_byte(FrameFragmentType::None)
  {
    resetMessageBuffers();
  };
  ~Protocol() {};

  void processByte(uint8_t byte);

private:

  ParsedFrameResultType processFrameByte(uint8_t byte);

  bool checkChecksum(uint8_t checksum);
  void parseFrameAndSetAnimation();


  void sendACK();
  void sendNACK();

  void resetMessageBuffers();

  AnimationManager& am;

  uint8_t current_frame_len;
  uint8_t frame_bytes_received;
  
  uint8_t frame_buffer[MAX_FRAME_BUFFER];

  CommFragmentType awaiting_comm_byte;
  FrameFragmentType awaiting_frame_byte;
};

#endif
