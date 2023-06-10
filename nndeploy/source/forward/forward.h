
#ifndef _NNDEPLOY_SOURCE_FORWARD_FORWARD_H_
#define _NNDEPLOY_SOURCE_FORWARD_FORWARD_H_

#include "nndeploy/source/base/basic.h"
#include "nndeploy/source/base/log.h"
#include "nndeploy/source/base/macro.h"
#include "nndeploy/source/base/object.h"
#include "nndeploy/source/base/status.h"
#include "nndeploy/source/device/device.h"
#include "nndeploy/source/forward/forward_impl.h"

namespace nndeploy {
namespace forward {

class ForwardConfig {
 public:
  base::PrecisionType precision_ = base::kPrecisionTypeFp32;
  base::PowerType power_type_ = base::kPowerTypeNormal;

  bool is_dynamic_shape_ = false;
  bool is_quant_ = false;
};

class Forward {
 public:
  Forward();
  ~Forward();

  /**
   * @brief Set the Device object
   * # 在初始化前必须调用setDevice
   * # 可以多次调用该函数
   * ## 自动子图拆分，异构执行
   * @param device
   * @return base::Status
   */
  base::Status setDevice(device::Device *device);
  device::Device *getDevice();
  device::Device *getDevice(base::DeviceType device_type);

  /**
   * @brief Set the Memory Pool object
   * # 在初始化前必须调用setBufferPool
   * # 可以多次调用该函数
   * ## 自动子图拆分，支持异构执行的内存分配
   * @param buffer_pool
   * @return base::Status
   * @note
   * # BufferPool中的device不在std::vector<device::Device *>
   * devices_的话，该数据无意义
   */
  base::Status setBufferPool(device::BufferPool *buffer_pool);
  device::BufferPool *getBufferPool();
  device::BufferPool *getBufferPool(base::DeviceType device_type);

  base::Status setShareMemoryType(base::ShareMemoryType share_memory_type);

  /**
   * @brief 执行顺序
   * # 初始化参数
   * # 绑定op
   * ## oneDnn
   * ## xnnpack
   * ## qnnpack
   * # op的初始化
   * # op的prerun
   * # 图优化 运行时内存分配
   * # 内存分配
   *
   * @param interpret
   * @param config
   * @param min_shape
   * @param opt_shape
   * @param max_shape
   * @return base::Status
   */
  base::Status init(interpret::Interpret *interpret, ForwardConfig config,
                    base::ShapeMap min_shape = base::ShapeMap(),
                    base::ShapeMap opt_shape = base::ShapeMap(),
                    base::ShapeMap max_shape = base::ShapeMap());
  base::Status deinit();

  ForwardConfig getConfig();

  base::Status getCurentShape(base::ShapeMap &shape_map);
  base::Status getMinShape(base::ShapeMap &shape_map);
  base::Status getOptShape(base::ShapeMap &shape_map);
  base::Status getMaxShape(base::ShapeMap &shape_map);

  base::Status checkDynamicShape(base::ShapeMap &shape_map);
  base::Status reshape(base::ShapeMap &shape_map);

  int64_t getShareMemorySize();
  int64_t getShareMemorySize(base::DeviceType device_type);
  base::Status setShareMemory(device::Buffer *buffer);

  TensorMap getAllInputTensor();
  TensorMap getAllOutputTensor();

  int getNumOfInputTensor();
  int getNumOfOutputTensor();

  std::vector<std::string> getAllInputTensorName();
  std::vector<std::string> getAllOutputTensorName();

  std::shared_ptr<Tensor> getInputTensor(const std::string &key);
  std::shared_ptr<Tensor> getOutputTensor(const std::string &key);

  base::Status setInputTensor(const std::string &key,
                              const std::shared_ptr<Tensor> input_tensor);
  //
  std::shared_ptr<Tensor> getOutputTensor(const std::string &key,
                                          std::vector<int32_t> config);

  base::Status run();
  base::Status asyncRun();

  std::shared_ptr<AbstractForwardImpl> getForwardImpl();

 private:
  std::shared_ptr<ForwardImpl> forward_impl_;
};

}  // namespace forward
}  // namespace nndeploy

#endif