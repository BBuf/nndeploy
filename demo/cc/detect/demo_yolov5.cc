#include "nndeploy/base/glic_stl_include.h"
#include "nndeploy/base/time_profiler.h"
#include "nndeploy/model/detect/opencv/detr/detr.h"
#include "nndeploy/pipeline/task.h"

using namespace nndeploy;

cv::Mat draw_box(cv::Mat &cv_mat, model::DetectResults &results) {
  // float w_ratio = float(cv_mat.cols) / float(640);
  // float h_ratio = float(cv_mat.rows) / float(640);
  float w_ratio = float(cv_mat.cols);
  float h_ratio = float(cv_mat.rows);
  const int CNUM = 80;
  cv::RNG rng(0xFFFFFFFF);
  cv::Scalar_<int> randColor[CNUM];
  for (int i = 0; i < CNUM; i++)
    rng.fill(randColor[i], cv::RNG::UNIFORM, 0, 256);
  int i = -1;
  for (auto result : results.result_) {
    std::array<float, 4> box;
    box[0] = result.bbox_[0];
    box[2] = result.bbox_[2];
    box[1] = result.bbox_[1];
    box[3] = result.bbox_[3];
    NNDEPLOY_LOGE("box[0]:%f, box[2]:%f, box[1]:%f, box[3]:%f\n", box[0],
                  box[2], box[1], box[3]);
    box[0] *= w_ratio;
    box[2] *= w_ratio;
    box[1] *= h_ratio;
    box[3] *= h_ratio;
    int width = box[2] - box[0];
    int height = box[3] - box[1];
    int id = result.label_id_;
    cv::Point p = cv::Point(box[0], box[1]);
    cv::Rect rect = cv::Rect(box[0], box[1], width, height);
    cv::rectangle(cv_mat, rect, randColor[id]);
    std::string text = " ID:" + std::to_string(id);
    cv::putText(cv_mat, text, cv::Point(0, 0), cv::FONT_HERSHEY_PLAIN, 1,
                randColor[id]);
  }
  return cv_mat;
}

int main(int argc, char *argv[]) {
  std::string name = "yolov5s";
  base::InferenceType type = base::kInferenceTypeMnn;
  bool is_path = true;//?
  std::vector<std::string> model_value;
  model_value.push_back(
      "/home/forever/github/tnn-models/model/yolov5/");
  pipeline::Packet input("yolo_in");
  pipeline::Packet output("yolo_out");
  pipeline::Pipeline *pipeline = model::opencv::creatDetrPipeline(
      name, type, &input, &output, true, model_value);
  if (pipeline == nullptr) {
    NNDEPLOY_LOGE("pipeline is nullptr");
    return -1;
  }

  NNDEPLOY_TIME_POINT_START("pipeline->init()");
  pipeline->init();
  NNDEPLOY_TIME_POINT_END("pipeline->init()");

  cv::Mat input_mat =
      cv::imread("/home/always/github/TensorRT-DETR/cpp/res.jpg");
  input.set(input_mat);

  NNDEPLOY_TIME_POINT_START("pipeline->run()");
  for (int i = 0; i < 10; ++i) {
    pipeline->run();
  }
  NNDEPLOY_TIME_POINT_END("pipeline->run()");

  model::DetectResults *results = (model::DetectResults *)output.getParam();
  NNDEPLOY_LOGE("results->size() = %d\n", results->result_.size());
  draw_box(input_mat, *results);
  cv::imwrite("/home/always/github/TensorRT-DETR/cpp/res_again.jpg", input_mat);

//没明白
  std::ofstream oss;
  oss.open("detr.dot", std::ios::out);
  pipeline->dump(oss);

  pipeline->deinit();

  delete pipeline;

  NNDEPLOY_TIME_PROFILER_PRINT();

  printf("hello world!\n");
  return 0;
}