/**
 * @file runtime.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _NNDEPLOY_INCLUDE_DEVICE_BUFFER_H_
#define _NNDEPLOY_INCLUDE_DEVICE_BUFFER_H_

#include "nndeploy/include/base/basic.h"
#include "nndeploy/include/base/object.h"
#include "nndeploy/include/base/status.h"


namespace nndeploy {
namespace device {

class Device;
class BufferPool;

struct BufferDesc {
  BufferDesc() : memory_buffer_type_(base::MEMORY_BUFFER_TYPE_1D){};
  BufferDesc(size_t size) : memory_buffer_type_(base::MEMORY_BUFFER_TYPE_1D) {
    size_.push_back(size);
  };
  BufferDesc(base::MemoryBufferType memory_buffer_type, base::SizeVector size)
      : memory_buffer_type_(memory_buffer_type), size_(size){};
  BufferDesc(base::MemoryBufferType memory_buffer_type, size_t *size,
             size_t len)
      : memory_buffer_type_(memory_buffer_type) {
    for (int i = 0; i < len; ++i) {
      size_.push_back(size[i]);
    }
  };
  BufferDesc(base::MemoryBufferType memory_buffer_type, base::SizeVector size,
             base::IntVector config)
      : memory_buffer_type_(memory_buffer_type), size_(size), config_(config){};
  BufferDesc(base::MemoryBufferType memory_buffer_type, size_t *size,
             size_t len, base::IntVector config)
      : memory_buffer_type_(memory_buffer_type), config_(config) {
    for (int i = 0; i < len; ++i) {
      size_.push_back(size[i]);
    }
  };

  base::MemoryBufferType memory_buffer_type_;
  /**
   * @brief
   * 1d size
   * 2d h w c
   * 3d unknown
   */
  base::SizeVector size_;
  /**
   * @brief
   * 根据不同的设备以及内存形态有不同的config_
   */
  base::IntVector config_;
};

class Buffer : public base::NonCopyable {
  friend class Device;
  friend class BufferPool;

 public:
  // get
  bool empty();
  base::DeviceType getDeviceType();
  Device *getDevice();
  BufferPool *getBufferPool();
  bool isBufferPool();
  bool isExternal();
  BufferDesc getDesc();
  base::MemoryBufferType getMemoryBufferType();
  size_t getSize();
  base::SizeVector getSizeVector();
  base::IntVector getConfig();
  void *getPtr();
  int32_t getId();

 private:
  virtual ~Buffer(){};

  Buffer(Device *device, size_t size, void *ptr, bool is_external)
      : device_(device),
        buffer_pool_(nullptr),
        desc_(size),
        data_ptr_(ptr),
        data_id_(-1),
        is_external_(is_external){};
  Buffer(Device *device, size_t size, int32_t id, bool is_external)
      : device_(device),
        buffer_pool_(nullptr),
        desc_(size),
        data_ptr_(nullptr),
        data_id_(id),
        is_external_(is_external){};
  Buffer(Device *device, BufferDesc desc, void *ptr, bool is_external)
      : device_(device),
        buffer_pool_(nullptr),
        desc_(desc),
        data_ptr_(ptr),
        data_id_(-1),
        is_external_(is_external){};
  Buffer(Device *device, BufferDesc desc, int32_t id, bool is_external)
      : device_(device),
        buffer_pool_(nullptr),
        desc_(desc),
        data_ptr_(nullptr),
        data_id_(id),
        is_external_(is_external){};

  Buffer(BufferPool *buffer_pool, size_t size, void *ptr)
      : device_(nullptr),
        buffer_pool_(buffer_pool),
        desc_(size),
        data_ptr_(ptr),
        data_id_(-1),
        is_external_(false){};
  Buffer(BufferPool *buffer_pool, size_t size, int32_t id)
      : device_(nullptr),
        buffer_pool_(buffer_pool),
        desc_(size),
        data_ptr_(nullptr),
        data_id_(id),
        is_external_(false){};
  Buffer(BufferPool *buffer_pool, BufferDesc desc, void *ptr)
      : device_(nullptr),
        buffer_pool_(buffer_pool),
        desc_(desc),
        data_ptr_(ptr),
        data_id_(-1),
        is_external_(false){};
  Buffer(BufferPool *buffer_pool, BufferDesc desc, int32_t id)
      : device_(nullptr),
        buffer_pool_(buffer_pool),
        desc_(desc),
        data_ptr_(nullptr),
        data_id_(id),
        is_external_(false){};

 private:
  Device *device_ = nullptr;
  BufferPool *buffer_pool_ = nullptr;
  BufferDesc desc_;
  void *data_ptr_ = nullptr;
  int32_t data_id_ = -1;
  bool is_external_ = false;
};

}  // namespace device
}  // namespace nndeploy

#endif