
#ifndef _NNDEPLOY_SOURCE_TASK_TASK_H_
#define _NNDEPLOY_SOURCE_TASK_TASK_H_

#include "nndeploy/source/base/basic.h"
#include "nndeploy/source/base/glic_stl_include.h"
#include "nndeploy/source/base/log.h"
#include "nndeploy/source/base/macro.h"
#include "nndeploy/source/base/object.h"
#include "nndeploy/source/base/status.h"
#include "nndeploy/source/base/string.h"
#include "nndeploy/source/base/value.h"
#include "nndeploy/source/device/buffer.h"
#include "nndeploy/source/device/buffer_pool.h"
#include "nndeploy/source/device/device.h"
#include "nndeploy/source/device/tensor.h"
#include "nndeploy/source/inference/inference.h"
#include "nndeploy/source/inference/inference_param.h"
#include "nndeploy/source/task/execution.h"
#include "nndeploy/source/task/packet.h"

namespace nndeploy {
namespace task {

class Task : public Execution {
 public:
  Task(base::InferenceType type, base::DeviceType device_type,
       const std::string &name = "");
  virtual ~Task();

  template <typename T>
  base::Status createPreprocess() {
    pre_process_ = dynamic_cast<Execution *>(new T(device_type_, name_));
    if (pre_process_ == nullptr) {
      return base::kStatusCodeErrorOutOfMemory;
    } else {
      return base::kStatusCodeOk;
    }
  }
  template <typename T>
  base::Status createPostprocess() {
    post_process_ = dynamic_cast<Execution *>(new T(device_type_, name_));
    if (post_process_ == nullptr) {
      return base::kStatusCodeErrorOutOfMemory;
    } else {
      return base::kStatusCodeOk;
    }
  }

  base::Param *getPreProcessParam();
  base::Param *getInferenceParam();
  base::Param *getPostProcessParam();

  virtual base::Status init();
  virtual base::Status deinit();

  virtual base::Status setInput(Packet &input);
  virtual base::Status setOutput(Packet &output);

  virtual base::Status run();

 private:
  base::Status allocateInferenceInputOutput();
  base::Status deallocateInferenceInputOutput();

 protected:
  base::InferenceType type_;
  Execution *pre_process_ = nullptr;
  std::vector<device::Tensor *> input_tensors_;
  Packet *inference_input_packet_;
  inference::Inference *inference_ = nullptr;
  std::vector<device::Tensor *> output_tensors_;
  Packet *inference_output_packet_;
  Execution *post_process_ = nullptr;
};

using creteTaskFunc = Task *(*)(base::InferenceType type,
                                base::DeviceType device_type,
                                const std::string &name, bool model_is_path,
                                std::vector<std::string> model_value);

std::map<std::string, creteTaskFunc> &getGlobalTaskCreatorMap();

class TypeTaskRegister {
 public:
  explicit TypeTaskRegister(const std::string &name, creteTaskFunc func) {
    getGlobalTaskCreatorMap()[name] = func;
  }
};

Task *creteTask(base::InferenceType type, base::DeviceType device_type,
                const std::string &name, bool model_is_path,
                std::vector<std::string> model_value);

}  // namespace task
}  // namespace nndeploy

#endif