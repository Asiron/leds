#ifndef PROTOCOL_HEADER_GUARD
#define PROTOCOL_HEADER_GUARD

#define DroneSerial Serial2

#define MAX_FRAME_BUFFER 128
#define MAX_MSG_BUFFER   128


enum ASCIICodes { 
  NUL = 0, 
  STX = 2, 
  ETX = 3,
  EOT = 4,
  ENQ = 5,
  ACK = 6
  NACK = 21
};

#include "AnimationManager.h"

class AnimationManager;

class Protocol 
{


  struct CommFragmentType
  {
      enum Type
      {
          Enquiry = ENQ, 
          MessageByte = NUL,
          EndOfTransmission = EOT
      };
      Type t_;
      CommFragmentType(Type t) : t_(t) {}
      operator Type () const {return t_;}

  private:
     //prevent automatic conversion
     template<typename T> operator T () const;
  };

  struct FrameFragmentType
  {
      enum Type
      {
          None = NUL,
          StartOfFrame = STX,
          FrameLength, 
          FrameByte, 
          Checksum, 
          EndOfFrame = ETX
      };
      Type t_;
      FrameFragmentType(Type t) : t_(t) {}
      operator Type () const {return t_;}

  private:
     //prevent automatic conversion
     template<typename T> operator T () const;
  };


  struct ParsedFrameResultType
  {
      enum Type
      {
        Success, NotFinished, Failure
      };
      Type t_;
      ParsedFrameResultType(Type t) : t_(t) {}
      operator Type () const {return t_;}

  private:
     //prevent automatic conversion
     template<typename T> operator T () const;
  };


public:
  Protocol(const AnimationManager& am): _am(am), current_msg_len(0), message_bytes_received(0){};
  ~Protocol();

  void processByte(int8_t byte);

private:

  ParsedFrameResultType processFrameByte(int8_t byte);

  void sendACK();
  void sendNACK();

  void resetMessageBuffers();

  const AnimationManager& am;

  CommFragmentType awaiting_comm_byte;
  FrameFragmentType awaiting_frame_byte;

  int8_t current_frame_len;
  int8_t frame_bytes_received;
  
  int8_t frame_buffer[MAX_FRAME_BUFFER];

};

#endif