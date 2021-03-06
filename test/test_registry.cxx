#include "fdp.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/datapipeline.hxx"
#include "fdp/registry/file_system.hxx"
#include "gtest/gtest.h"
#include <filesystem>
#include <ostream>
#include <vector>

using namespace FDP;

DataPipelineImpl_ *init_pipeline(bool use_local = false) {
  if (std::filesystem::exists(std::filesystem::path(TESTDIR) /
                              std::filesystem::path("datastore"))) {
    std::filesystem::remove_all(std::filesystem::path(TESTDIR) /
                                std::filesystem::path("datastore"));
  }

  const std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "config.yaml";
  APILogger->set_level(spdlog::level::debug);

  return new DataPipelineImpl_(config_path_, "", spdlog::level::debug,
                               static_cast<RESTAPI>(use_local));
}

TEST(FDPAPITest, TestDataPipelineInit) {
  if (std::filesystem::exists(std::filesystem::path(TESTDIR) /
                              std::filesystem::path("datastore"))) {
    std::filesystem::remove_all(std::filesystem::path(TESTDIR) /
                                std::filesystem::path("datastore"));
  }

  const std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "config.yaml";
  APILogger->set_level(spdlog::level::debug);
  DataPipeline(config_path_, "", spdlog::level::debug);
}

TEST(FDPAPITest, TestLogLevelSet) {
  init_pipeline();
  ASSERT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST(FDPAPITest, TestURLEncode) {
  const std::string start_string_ = "fixed-parameters/T_lat";
  const std::string expected_ = "fixed-parameters%2FT_lat";

  ASSERT_EQ(url_encode(start_string_), expected_);
}

TEST(FDPAPITest, TestAccessObjectRegistry) {
  ASSERT_NO_THROW(init_pipeline()->fetch_all_objects());
}

TEST(FDPAPITest, TestAccessObject) {
  ASSERT_NO_THROW(init_pipeline()->fetch_object_by_id(72974));
}

TEST(FDPAPITest, TestAccessDataRegistry) {
  ASSERT_NO_THROW(init_pipeline()->fetch_all_data_products());
}

TEST(FDPAPITest, TestAccessData) {
  ASSERT_NO_THROW(init_pipeline()->fetch_object_by_id(3337));
}

TEST(FDPAPITest, TestFileSystemSetup) {
  LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
  ASSERT_TRUE(
      std::filesystem::exists(std::filesystem::path(TESTDIR) / "datastore"));
}

TEST(FDPAPITest, TestGetConfigDataTOML) {
  LocalFileSystem *fs_ =
      new LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
  const std::vector<ReadObject::DataProduct *> data_products_ =
      fs_->read_data_products();
  ASSERT_TRUE(data_products_[0]);
  const Versioning::version version_ = data_products_[0]->get_version();
  const std::filesystem::path path_ = data_products_[0]->get_path();
  ASSERT_EQ(version_, Versioning::version(0, 1, 0));
  ASSERT_EQ(path_, "fixed-parameters/T_lat");
}

TEST(FDPAPITest, TestGetConfigDataHDF5) {
  LocalFileSystem *fs_ =
      new LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
  const std::vector<ReadObject::DataProduct *> data_products_ =
      fs_->read_data_products();
  ASSERT_TRUE(data_products_[1]);
  const Versioning::version version_ = data_products_[1]->get_version();
  const std::filesystem::path path_ = data_products_[1]->get_path();
  ASSERT_EQ(version_, Versioning::version(0, 20200813, 0));
  ASSERT_EQ(path_, "prob_hosp_and_cfr/data_for_scotland");
}

TEST(FDPAPITest, TestHashFile) {
  const std::string file_hash_ =
      calculate_hash_from_file(std::filesystem::path(TESTDIR) / "config.yaml");
  std::cout << "HASH: " << file_hash_ << std::endl;
}

TEST(FDPAPITest, TestDownloadTOMLFile) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline();
  const std::vector<ReadObject::DataProduct *> data_products_ =
      data_pipeline_->file_system->read_data_products();
  ASSERT_TRUE(data_products_[0]);
  ASSERT_NO_THROW(data_pipeline_->download_data_product(data_products_[0]));
}

TEST(FDPAPITest, TestDownloadHDF5File) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline();
  const std::vector<ReadObject::DataProduct *> data_products_ =
      data_pipeline_->file_system->read_data_products();
  ASSERT_NO_THROW(data_pipeline_->download_data_product(data_products_[1]));
}

TEST(FDPAPITest, DISABLED_TestAddToRegister) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline(true);
  data_pipeline_->add_to_register("raw-mortality-data");
}

TEST(FDPAPITest, TestDownloadExternalObject) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline();
  const std::vector<ReadObject::ExternalObject *> external_objs =
      data_pipeline_->file_system->read_external_objects();

  ASSERT_TRUE(external_objs[0]);
  ASSERT_NO_THROW(data_pipeline_->download_external_object(external_objs[0]));
}