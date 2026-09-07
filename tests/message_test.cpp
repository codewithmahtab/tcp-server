#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "message.hpp"


TEST(MessageTest, MessageTypeValues) {

    EXPECT_EQ(
        static_cast<uint32_t>(MessageType::TEXT),
        1
    );

    EXPECT_EQ(
        static_cast<uint32_t>(MessageType::PING),
        2
    );

    EXPECT_EQ(
        static_cast<uint32_t>(MessageType::RESPONSE),
        3
    );
}


TEST(MessageTest, SendAndReceiveMessage) {

    int sockets[2];


    ASSERT_EQ(
        socketpair(
            AF_UNIX,
            SOCK_STREAM,
            0,
            sockets
        ),
        0
    );


    Message original{
        MessageType::TEXT,
        "Hello from unit test"
    };


    Message received;


    ASSERT_TRUE(
        sendMessage(
            sockets[0],
            original
        )
    );


    ASSERT_TRUE(
        receiveMessage(
            sockets[1],
            received
        )
    );


    EXPECT_EQ(
        received.type,
        original.type
    );


    EXPECT_EQ(
        received.payload,
        original.payload
    );


    close(sockets[0]);

    close(sockets[1]);
}


TEST(MessageTest, RejectsInvalidMessageType) {

    int sockets[2];


    ASSERT_EQ(
        socketpair(
            AF_UNIX,
            SOCK_STREAM,
            0,
            sockets
        ),
        0
    );


    // Message header:
    // 4 bytes = message type
    // 4 bytes = payload length

    uint32_t invalidType = htonl(999);

    uint32_t payloadLength = htonl(0);


    ASSERT_EQ(
        send(
            sockets[0],
            &invalidType,
            sizeof(invalidType),
            0
        ),
        sizeof(invalidType)
    );


    ASSERT_EQ(
        send(
            sockets[0],
            &payloadLength,
            sizeof(payloadLength),
            0
        ),
        sizeof(payloadLength)
    );


    Message received;


    EXPECT_FALSE(
        receiveMessage(
            sockets[1],
            received
        )
    );


    close(sockets[0]);

    close(sockets[1]);
}


TEST(MessageTest, RejectsOversizedMessage) {

    int sockets[2];


    ASSERT_EQ(
        socketpair(
            AF_UNIX,
            SOCK_STREAM,
            0,
            sockets
        ),
        0
    );


    uint32_t messageType =
        htonl(
            static_cast<uint32_t>(
                MessageType::TEXT
            )
        );


    // 1 MB payload.
    // Our protocol should reject this
    // because MAX_MESSAGE_SIZE is 4096.
    uint32_t oversizedLength = htonl(1024 * 1024);


    ASSERT_EQ(
        send(
            sockets[0],
            &messageType,
            sizeof(messageType),
            0
        ),
        sizeof(messageType)
    );


    ASSERT_EQ(
        send(
            sockets[0],
            &oversizedLength,
            sizeof(oversizedLength),
            0
        ),
        sizeof(oversizedLength)
    );


    Message received;


    EXPECT_FALSE(
        receiveMessage(
            sockets[1],
            received
        )
    );


    close(sockets[0]);

    close(sockets[1]);
}