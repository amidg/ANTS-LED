#ifndef _ROS_SERVICE_SetAccelNoise_h
#define _ROS_SERVICE_SetAccelNoise_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/Vector3.h"

namespace microstrain_mips
{

static const char SETACCELNOISE[] = "microstrain_mips/SetAccelNoise";

  class SetAccelNoiseRequest : public ros::Msg
  {
    public:
      typedef geometry_msgs::Vector3 _noise_type;
      _noise_type noise;

    SetAccelNoiseRequest():
      noise()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->noise.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->noise.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return SETACCELNOISE; };
    const char * getMD5(){ return "722ffe5b1a889deb606916858b9a7f4f"; };

  };

  class SetAccelNoiseResponse : public ros::Msg
  {
    public:
      typedef bool _success_type;
      _success_type success;

    SetAccelNoiseResponse():
      success(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_success;
      u_success.real = this->success;
      *(outbuffer + offset + 0) = (u_success.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->success);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_success;
      u_success.base = 0;
      u_success.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->success = u_success.real;
      offset += sizeof(this->success);
     return offset;
    }

    const char * getType(){ return SETACCELNOISE; };
    const char * getMD5(){ return "358e233cde0c8a8bcfea4ce193f8fc15"; };

  };

  class SetAccelNoise {
    public:
    typedef SetAccelNoiseRequest Request;
    typedef SetAccelNoiseResponse Response;
  };

}
#endif
