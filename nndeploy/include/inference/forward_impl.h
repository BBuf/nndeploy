/**
 * @file abstract_model.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _NNDEPLOY_INCLUDE_INFERENCE_ABSTRACT_MODEL_
#define _NNDEPLOY_INCLUDE_INFERENCE_ABSTRACT_MODEL_

#include "nndeploy/include/base/config.h"
#include "nndeploy/include/base/status.h"

using namespace nndeploy::base;
using namespace nndeploy::device;

namespace nndeploy {
namespace inference {

class ForwardImpl {
 public:
  explicit ForwardImpl(Param param);

  virtual ~ForwardImpl();

  Status SetConfig(const std::string &key, nndeploy::base::Value value);
  Param GetParam();

  bool isConstract();

  virtual Status Init(ShapeMap min_shape = ShapeMap(), ShapeMap opt_shape = ShapeMap(),
      ShapeMap max_shape = ShapeMap()) = 0;
  virtual Status Deinit() = 0;

  virtual Status GetStaticShape(ShapeMap shape_map);
  virtual Status GetMinShape(ShapeMap &shape_map);
  virtual Status GetOptShape(ShapeMap &shape_map);
  virtual Status GetCurentShape(ShapeMap &shape_map);
  virtual Status GetMaxShape(ShapeMap &shape_map);

  virtual Status ReShape(ShapeMap &shape_map);

  virtual Stauts SetDevice(DevicePacket *device);
  virtual Device *GetDevice();

  virtual Stauts SetWorkspace(Buffer *buffer);
  virtual int64_t GetWorkspaceSize();

  virtual Stauts SetMemory(Buffer *buffer);
  virtual int64_t GetMemorySize();

  // 得到内部分配的内存
  virtual Status GetAllInputTensor(TensorMap input_tensors);
  virtual Status GetAllOutputTensor(TensorMap output_tensors);

  virtual int GetNumOfInputTensor();
  virtual int GetNumOfOutputTensor();

  virtual std::vector<std::string> GetInputTensorNames();
  virtual std::vector<std::string> GetOutputTensorNames();

  virtual std::shared_ptr<Tensor> GetInputTensor(const string &key);
  virtual std::shared_ptr<Tensor> GetOutputTensor(const string &key);

  // 外部分配内存
  virtual Status SetInputTensor(const string &key,
                                const std::shared_ptr<Tensor> input_tensor);
  //
  virtual std::shared_ptr<Tensor> GetOutputTensor(const string &key,
                                                  std::vector<int32_t> config);

  virtual Status Run();
  virtual Stauts AsyncRun();

 protected:
  InferenceConfig inference_config_;
  ShapeMap static_shape_map_;
  bool is_construct_flag_= false;

  ShapeMap current_shape_ = ShapeMap();
  ShapeMap min_shape_ = ShapeMap();
  ShapeMap opt_shape_ = ShapeMap();
  ShapeMap max_shape_ = ShapeMap();

  TensorMap input_tensors; // 完全与推理框架保持一致的
  TensorMap output_tensors;

  /**
   * @brief 内部不管理该部分资源，由外部导入
   * # 内存资源
   * # 设备运行时资源
   */
  Device* device_ = nullptr;  // 通常cpu runtime 不用导入
};

}  // namespace inference
}  // namespace nndeploy

#endif