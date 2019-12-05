#include <gtest/gtest.h>
#include <roc/log.h>

int main(int argc, char** argv) {
    roc_log_set_level(ROC_LOG_INFO);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
