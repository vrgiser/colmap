// COLMAP - Structure-from-Motion and Multi-View Stereo.
// Copyright (C) 2017  Johannes L. Schoenberger <jsch at inf.ethz.ch>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "base/camera_models.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>

namespace colmap {

// Initialize params_info, focal_length_idxs, principal_point_idxs,
// extra_params_idxs
#define CAMERA_MODEL_CASE(CameraModel)                          \
  const int CameraModel::model_id;                              \
  const std::string CameraModel::model_name =                   \
      CameraModel::InitializeModelName();                       \
  const int CameraModel::num_params;                            \
  const std::string CameraModel::params_info =                  \
      CameraModel::InitializeParamsInfo();                      \
  const std::vector<size_t> CameraModel::focal_length_idxs =    \
      CameraModel::InitializeFocalLengthIdxs();                 \
  const std::vector<size_t> CameraModel::principal_point_idxs = \
      CameraModel::InitializePrincipalPointIdxs();              \
  const std::vector<size_t> CameraModel::extra_params_idxs =    \
      CameraModel::InitializeExtraParamsIdxs();

CAMERA_MODEL_CASES

#undef CAMERA_MODEL_CASE

std::unordered_map<std::string, int> InitialzeCameraModelNameToId() {
  std::unordered_map<std::string, int> camera_model_name_to_id;

#define CAMERA_MODEL_CASE(CameraModel)                     \
  camera_model_name_to_id.emplace(CameraModel::model_name, \
                                  CameraModel::model_id);

  CAMERA_MODEL_CASES

#undef CAMERA_MODEL_CASE

  return camera_model_name_to_id;
}

static const std::unordered_map<std::string, int> CAMERA_MODEL_NAME_TO_ID =
    InitialzeCameraModelNameToId();

int CameraModelNameToId(const std::string& model_name) {
  if (CAMERA_MODEL_NAME_TO_ID.count(model_name)) {
    return CAMERA_MODEL_NAME_TO_ID.at(model_name);
  } else {
    return kInvalidCameraModelId;
  }
}

std::unordered_map<int, std::string> InitialzeCameraModelIdToName() {
  std::unordered_map<int, std::string> camera_model_id_to_name;

#define CAMERA_MODEL_CASE(CameraModel)                   \
  camera_model_id_to_name.emplace(CameraModel::model_id, \
                                  CameraModel::model_name);

  CAMERA_MODEL_CASES

#undef CAMERA_MODEL_CASE

  return camera_model_id_to_name;
}

static const std::unordered_map<int, std::string> CAMERA_MODEL_ID_TO_NAME =
    InitialzeCameraModelIdToName();

std::string CameraModelIdToName(const int model_id) {
  if (CAMERA_MODEL_ID_TO_NAME.count(model_id)) {
    return CAMERA_MODEL_ID_TO_NAME.at(model_id);
  } else {
    return "INVALID_CAMERA_MODEL";
  }
}

void CameraModelInitializeParams(const int model_id, const double focal_length,
                                 const size_t width, const size_t height,
                                 std::vector<double>* params) {
  // Assuming that image measurements are within [0, dim], i.e. that the
  // upper left corner is the (0, 0) coordinate (rather than the center of
  // the upper left pixel). This complies with the default SiftGPU convention.
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel)                              \
  case CameraModel::model_id:                                       \
    params->resize(CameraModel::num_params);                        \
    for (const int idx : CameraModel::focal_length_idxs) {          \
      (*params)[idx] = focal_length;                                \
    }                                                               \
    (*params)[CameraModel::principal_point_idxs[0]] = width / 2.0;  \
    (*params)[CameraModel::principal_point_idxs[1]] = height / 2.0; \
    for (const int idx : CameraModel::extra_params_idxs) {          \
      (*params)[idx] = 0;                                           \
    }                                                               \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }
}

std::string CameraModelParamsInfo(const int model_id) {
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel) \
  case CameraModel::model_id:          \
    return CameraModel::params_info;   \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }

  return "Camera model does not exist";
}

std::vector<size_t> CameraModelFocalLengthIdxs(const int model_id) {
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel)     \
  case CameraModel::model_id:              \
    return CameraModel::focal_length_idxs; \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }

  return std::vector<size_t>{};
}

std::vector<size_t> CameraModelPrincipalPointIdxs(const int model_id) {
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel)        \
  case CameraModel::model_id:                 \
    return CameraModel::principal_point_idxs; \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }

  return std::vector<size_t>{};
}

std::vector<size_t> CameraModelExtraParamsIdxs(const int model_id) {
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel)     \
  case CameraModel::model_id:              \
    return CameraModel::extra_params_idxs; \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }

  return std::vector<size_t>{};
}

bool CameraModelVerifyParams(const int model_id,
                             const std::vector<double>& params) {
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel)              \
  case CameraModel::model_id:                       \
    if (params.size() == CameraModel::num_params) { \
      return true;                                  \
    }                                               \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }

  return false;
}

bool CameraModelHasBogusParams(const int model_id,
                               const std::vector<double>& params,
                               const size_t width, const size_t height,
                               const double min_focal_length_ratio,
                               const double max_focal_length_ratio,
                               const double max_extra_param) {
  switch (model_id) {
#define CAMERA_MODEL_CASE(CameraModel)                                         \
  case CameraModel::model_id:                                                  \
    return CameraModel::HasBogusParams(                                        \
        params, width, height, min_focal_length_ratio, max_focal_length_ratio, \
        max_extra_param);                                                      \
    break;

    CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
  }

  return false;
}

}  // namespace colmap
