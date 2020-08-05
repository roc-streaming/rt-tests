#include <gtest/gtest.h>

#include <roc/context.h>
#include <roc/receiver.h>
#include <roc/sender.h>

#include <algorithm>
#include <future>
#include <vector>

class ServiceQuality : public ::testing::Test {
protected:
    void SetUp() override {
        // create context
        roc_context_config context_config {};

        ASSERT_EQ(roc_context_open(&context_config, &context_), 0);
        ASSERT_NE(context_, nullptr);

        // create receiver
        roc_receiver_config receiver_config {};
        receiver_config.frame_sample_rate = 44100;
        receiver_config.frame_channels = ROC_CHANNEL_SET_STEREO;
        receiver_config.frame_encoding = ROC_FRAME_ENCODING_PCM_FLOAT;
        receiver_config.clock_source = ROC_CLOCK_INTERNAL;
        receiver_config.resampler_profile = ROC_RESAMPLER_PROFILE_DISABLE;

        ASSERT_EQ(roc_receiver_open(context_, &receiver_config, &receiver_), 0);
        ASSERT_NE(receiver_, nullptr);

        // bind receiver to two endpoints
        roc_endpoint* source_endp {};
        ASSERT_EQ(roc_endpoint_allocate(&source_endp), 0);
        ASSERT_EQ(roc_endpoint_set_protocol(source_endp, ROC_PROTO_RTP_RS8M_SOURCE), 0);
        ASSERT_EQ(roc_endpoint_set_host(source_endp, "127.0.0.1"), 0);
        ASSERT_EQ(roc_endpoint_set_port(source_endp, 0), 0);

        ASSERT_EQ(roc_receiver_bind(receiver_, ROC_INTERFACE_AUDIO_SOURCE, source_endp),
                  0);

        roc_endpoint* repair_endp {};
        ASSERT_EQ(roc_endpoint_allocate(&repair_endp), 0);
        ASSERT_EQ(roc_endpoint_set_protocol(repair_endp, ROC_PROTO_RS8M_REPAIR), 0);
        ASSERT_EQ(roc_endpoint_set_host(repair_endp, "127.0.0.1"), 0);
        ASSERT_EQ(roc_endpoint_set_port(repair_endp, 0), 0);

        ASSERT_EQ(roc_receiver_bind(receiver_, ROC_INTERFACE_AUDIO_REPAIR, repair_endp),
                  0);

        // create sender
        roc_sender_config sender_config {};
        sender_config.frame_sample_rate = 44100;
        sender_config.frame_channels = ROC_CHANNEL_SET_STEREO;
        sender_config.frame_encoding = ROC_FRAME_ENCODING_PCM_FLOAT;
        sender_config.clock_source = ROC_CLOCK_INTERNAL;
        sender_config.resampler_profile = ROC_RESAMPLER_PROFILE_DISABLE;
        sender_config.fec_code = ROC_FEC_RS8M;

        ASSERT_EQ(roc_sender_open(context_, &sender_config, &sender_), 0);
        ASSERT_NE(sender_, nullptr);

        // connect sender to receiver endpoints
        ASSERT_EQ(roc_sender_connect(sender_, ROC_INTERFACE_AUDIO_SOURCE, source_endp),
                  0);

        ASSERT_EQ(roc_sender_connect(sender_, ROC_INTERFACE_AUDIO_REPAIR, repair_endp),
                  0);

        // deallocate endpoint objects
        ASSERT_EQ(roc_endpoint_deallocate(source_endp), 0);
        ASSERT_EQ(roc_endpoint_deallocate(repair_endp), 0);
    }

    void TearDown() override {
        if (sender_) {
            EXPECT_EQ(roc_sender_close(sender_), 0);
        }

        if (receiver_) {
            EXPECT_EQ(roc_receiver_close(receiver_), 0);
        }

        if (context_) {
            EXPECT_EQ(roc_context_close(context_), 0);
        }
    }

    void Send(size_t frame_size, size_t num_frames) {
        std::vector<float> frame_buf(frame_size);

        std::fill(frame_buf.begin(), frame_buf.end(), 0.5);

        for (size_t n = 0; n < num_frames; n++) {
            roc_frame frame = {};
            frame.samples = &frame_buf[0];
            frame.samples_size = frame_buf.size() * sizeof(float);

            EXPECT_EQ(roc_sender_write(sender_, &frame), 0);
        }
    }

    void Receive(size_t frame_size, size_t num_frames) {
        std::vector<float> frame_buf(frame_size);

        bool received_something = false;

        for (size_t n = 0; n < num_frames; n++) {
            std::fill(frame_buf.begin(), frame_buf.end(), 0);

            roc_frame frame = {};
            frame.samples = &frame_buf[0];
            frame.samples_size = frame_buf.size() * sizeof(float);

            EXPECT_EQ(roc_receiver_read(receiver_, &frame), 0);

            if (!std::all_of(frame_buf.begin(), frame_buf.end(),
                             [](float f) { return f == 0; })) {
                received_something = true;
            }
        }

        // This is a very basic check that we have recevied at least one non-zero sample.
        // Instead of it, we should generate some sequence on sender (e.g. a sine wave),
        // and on receiver check that we received the same sequence, probably with leading
        // and trailing zeros.
        EXPECT_TRUE(received_something);
    }

    roc_context* context_ {};
    roc_receiver* receiver_ {};
    roc_sender* sender_ {};
};

TEST_F(ServiceQuality, NoLoss) {
    auto sending = std::async(std::launch::async, [=]() { Send(256, 500); });

    Receive(256, 500);

    sending.wait();
}
