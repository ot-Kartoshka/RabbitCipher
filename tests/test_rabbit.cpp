#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <fstream>
#include <filesystem>

#include "../include/Rabbit.hpp"
#include "../include/KeyManager.hpp"
#include "../include/RabbitManager.hpp"
#include "../include/Errors.hpp"

namespace fs = std::filesystem;

std::vector<uint8_t> hexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        if (hex[i] == ' ' || hex[i] == '\n' || hex[i] == '\r') { i--; continue; }
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}


TEST(RabbitTest, IsoVector1) {
    auto K = hexToBytes("00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
    auto IV = hexToBytes("00 00 00 00 00 00 00 00");
    auto expected_Z = hexToBytes("ED B7 05 67 37 5D CD 7C D8 95 54 F8 5E 27 A7 C6 8D 4A DC 70 32 29 8F 7B D4 EF F5 04 AC A6 29 5F 66 8F BF 47 8A DB 2B E5 1E 6C DE 29 2B 82 DE 2A B4 8D 2A C6 56 59 79 22 0E C9 09 A7 E7 57 60 98");

    Rabbit rabbit;
    rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));

    std::vector<uint8_t> actual_Z;
    for (int i = 0; i < 4; ++i) {
        std::array<uint8_t, 16> block;
        rabbit.Strm(block);
        actual_Z.insert(actual_Z.end(), block.begin(), block.end());
    }

    EXPECT_EQ(actual_Z, expected_Z) << "Збій на Векторі 1 (K=0, IV=0)";
}

TEST(RabbitTest, IsoVector2) {
    auto K = hexToBytes("00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
    auto IV = hexToBytes("00 01 02 03 04 05 06 07");
    auto expected_Z = hexToBytes("98 71 C7 BA 4E A3 08 07 CD AA 49 64 66 39 2D 2F 4A FF 43 55 EF 90 69 56 10 9B 96 65 97 8D AC ED 9B 7C 6F 7F C8 2C 67 D2 73 22 CB DE 9D B0 16 45 8C 38 2C 9C 7D 30 44 E6 52 0B B9 2A 13 53 C0 FF");

    Rabbit rabbit;
    rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));

    std::vector<uint8_t> actual_Z;
    for (int i = 0; i < 4; ++i) {
        std::array<uint8_t, 16> block;
        rabbit.Strm(block);
        actual_Z.insert(actual_Z.end(), block.begin(), block.end());
    }

    EXPECT_EQ(actual_Z, expected_Z) << "Збій на Векторі 2 (K=0, IV=Seq)";
}

TEST(RabbitTest, IsoVector3) {
    auto K = hexToBytes("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    auto IV = hexToBytes("00 00 00 00 00 00 00 00");
    auto expected_Z = hexToBytes(" A8 F7 E6 9B 69 40 A7 8D 13 6A 5C 15 4A 15 79 52 A6 E4 23 58 59 E3 02 20 EA 68 64 36 BB 38 EF 53 9C 29 40 55 6B 09 EC D7 FE A2 B0 AC 83 07 F1 69 62 65 A3 D6 44 28 1C 39 C9 CD 5E 1E 2F 9B E4 D0");

    Rabbit rabbit;
    rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));

    std::vector<uint8_t> actual_Z;
    for (int i = 0; i < 4; ++i) {
        std::array<uint8_t, 16> block;
        rabbit.Strm(block);
        actual_Z.insert(actual_Z.end(), block.begin(), block.end());
    }

    EXPECT_EQ(actual_Z, expected_Z) << "Збій на Векторі 3 (K=Seq, IV=0)";
}

TEST(RabbitTest, IsoVector4) {
    auto K = hexToBytes("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    auto IV = hexToBytes("00 01 02 03 04 05 06 07");
    auto expected_Z = hexToBytes("F2 89 19 DD A1 28 F8 F9 0A 30 34 6E 97 94 D2 B7 4C 69 A2 D9 91 37 27 BC 5A 30 18 E6 33 2A F7 F3 BE 3A C3 EF B3 68 F4 3A 4C B8 58 67 B8 1C 91 F9 24 29 0C 81 6B 8B 57 88 98 C5 7F B4 C0 BA 05 BD");

    Rabbit rabbit;
    rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));

    std::vector<uint8_t> actual_Z;
    for (int i = 0; i < 4; ++i) {
        std::array<uint8_t, 16> block;
        rabbit.Strm(block);
        actual_Z.insert(actual_Z.end(), block.begin(), block.end());
    }

    EXPECT_EQ(actual_Z, expected_Z) << "Збій на Векторі 4 (K=Seq, IV=Seq)";
}


TEST(RabbitTest, XorSelfReversible) {
    std::string message = "Бара Бара БЕРЕ, варвара файла непе";
    std::vector<uint8_t> plain(message.begin(), message.end());
    std::vector<uint8_t> data = plain;

    auto K = hexToBytes("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    auto IV = hexToBytes("00 01 02 03 04 05 06 07");

    Rabbit enc_rabbit, dec_rabbit;
    enc_rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));
    dec_rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));

    enc_rabbit.XorData(data, data);
    EXPECT_NE(data, plain);     
    dec_rabbit.XorData(data, data);
    EXPECT_EQ(data, plain);   
}

TEST(RabbitManagerTest, ConsistencyWithManual) {
    std::string message = "Бара Бара БЕРЕ, варвара файла непе";
    std::vector<uint8_t> original_plain(message.begin(), message.end());

    auto K_vec = hexToBytes("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    auto IV_vec = hexToBytes("00 01 02 03 04 05 06 07");

    std::span<const uint8_t, Rabbit::KEY_SIZE> K(K_vec.data(), 16);
    std::span<const uint8_t, Rabbit::IV_SIZE> IV(IV_vec.data(), 8);

    Rabbit core_engine;
    core_engine.Init(K, IV);

    std::vector<uint8_t> manual_cipher = original_plain;
    core_engine.XorData(manual_cipher, manual_cipher);

    RabbitManager manager;
    std::vector<uint8_t> manager_cipher = manager.EncryptData(original_plain, K, IV);


    EXPECT_EQ(manual_cipher, manager_cipher) << "Помилка: Менеджер зашифрував інакше, ніж вручну!";

    EXPECT_NE(manager_cipher, original_plain);

    std::vector<uint8_t> decrypted = manager.DecryptData(manager_cipher, K, IV);
    EXPECT_EQ(decrypted, original_plain) << "Помилка: Розшифровані дані не збігаються з оригіналом!";
}

TEST(KeyManagerTest, ReadKeyInvalidSize_ReturnsInvalidKeySize) {
    std::string badKeyFile = "test_bad_key.bin";

    std::ofstream out(badKeyFile, std::ios::binary);
    out << "1234567890";
    out.close();

    auto readRes = KeyManager::ReadKey(badKeyFile);
    ASSERT_FALSE(readRes.has_value());
    EXPECT_EQ(readRes.error(), Error::InvalidKeySize);

    fs::remove(badKeyFile);
}

TEST(RabbitManagerTest, EndToEndFileEncryption) {
    std::string inFile = "test_in.txt";
    std::string encFile = "test_enc.bin";
    std::string decFile = "test_dec.txt";
    std::string keyFile = "test_manager_key.bin";

    std::string secretData = "ТУРУТУРТУРТУР ТУТУТУТТУ РОАМ";
    std::ofstream out(inFile);
    out << secretData;
    out.close();

    RabbitManager manager;

    auto encRes = manager.Conductor(inFile, encFile, keyFile, true);
    ASSERT_TRUE(encRes.has_value());

    auto decRes = manager.Conductor(encFile, decFile, keyFile, false);
    ASSERT_TRUE(decRes.has_value());

    std::ifstream inDec(decFile);
    std::string resultData((std::istreambuf_iterator<char>(inDec)), std::istreambuf_iterator<char>());
    inDec.close();

    EXPECT_EQ(resultData, secretData);

    fs::remove(inFile); fs::remove(encFile); fs::remove(decFile); fs::remove(keyFile);
}

TEST(RabbitTest, InternalStates_Annex_C43) {
    auto K = hexToBytes("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    auto IV = hexToBytes("00 01 02 03 04 05 06 07");

    Rabbit rabbit;

    //  від S(-9) до S(0)
    rabbit.Init(std::span<const uint8_t, 16>(K.data(), 16), std::span<const uint8_t, 8>(IV.data(), 8));

    // S(0)
    uint32_t expected_X_S0[8] = { 0x987651C2, 0xFF5F0007, 0x5C48C79E, 0x661B3E75, 0x49247B9A, 0x3C7AA744, 0x4AEF3F40, 0xD117584E };
    uint32_t expected_C_S0[8] = { 0xEC7D2860, 0xDB3521DF, 0x8C634E58, 0x341E1E3B, 0x3B589605, 0x57F9ACEF, 0x41EF8921, 0x5FC680F5 };

    for (int j = 0; j < 8; ++j) {
        EXPECT_EQ(rabbit.state.X[j], expected_X_S0[j]) << "Помилка у змінній X[" << j << "] на стані S(0)";
        EXPECT_EQ(rabbit.state.C[j], expected_C_S0[j]) << "Помилка у змінній C[" << j << "] на стані S(0)";
    }
    EXPECT_EQ(rabbit.state.b, 1) << "Помилка carry-біта на стані S(0)";


    // S(1)
    std::array<uint8_t, 16> dummy_Z;
    rabbit.Strm(dummy_Z); 

    uint32_t expected_X_S1[8] = { 0x2A158BE4, 0xD93EC5A4, 0x298B7C1B, 0x01F4F70C, 0xE241E934, 0x0216D073, 0x72769563, 0x54BA8C75 };
    uint32_t expected_C_S1[8] = { 0x39B1FBAE, 0xAE8256B3, 0xC1369B8D, 0x8152F188, 0x0EA5CAD8, 0x8CCCFA24, 0x8F245C6E, 0x3313B5C8 };

    for (int j = 0; j < 8; ++j) {
        EXPECT_EQ(rabbit.state.X[j], expected_X_S1[j]) << "Помилка у змінній X[" << j << "] на стані S(1)";
        EXPECT_EQ(rabbit.state.C[j], expected_C_S1[j]) << "Помилка у змінній C[" << j << "] на стані S(1)";
    }
    EXPECT_EQ(rabbit.state.b, 1) << "Помилка carry-біта на стані S(1)";

    // S(2) 
    rabbit.Strm(dummy_Z); 

    uint32_t expected_X_S2[8] = { 0x46EC0492, 0xA4B5D46E, 0x7B374C9E, 0x93249F4E, 0xE93894EF, 0x6DDEC710, 0x2799B917, 0x7B0F0F20 };
    uint32_t expected_C_S2[8] = { 0x86E6CEFC, 0x81CF8B86, 0xF609E8C2, 0xCE87C4D5, 0xE1F2FFAB, 0xC1A04758, 0xDC592FBB, 0x0660EA9B };

    for (int j = 0; j < 8; ++j) {
        EXPECT_EQ(rabbit.state.X[j], expected_X_S2[j]) << "Помилка у змінній X[" << j << "] на стані S(2)";
        EXPECT_EQ(rabbit.state.C[j], expected_C_S2[j]) << "Помилка у змінній C[" << j << "] на стані S(2)";
    }
    EXPECT_EQ(rabbit.state.b, 1) << "Помилка carry-біта на стані S(2)";

    // S(3) 
    rabbit.Strm(dummy_Z); 

    uint32_t expected_X_S3[8] = { 0x98C27422, 0x0D5B5EC2, 0xFEEC9F8D, 0x423F7701, 0xE22AB517, 0x4E9CC418, 0xA7535E87, 0xF73E8572 };
    uint32_t expected_C_S3[8] = { 0xD41BA24A, 0x551CC059, 0x2ADD35F7, 0x1BBC9823, 0xB540347F, 0xF673948D, 0x298E0308, 0xD9AE1F6F };
    
    for (int j = 0; j < 8; ++j) {
        EXPECT_EQ(rabbit.state.X[j], expected_X_S3[j]) << "Помилка у змінній X[" << j << "] на стані S(3)";
        EXPECT_EQ(rabbit.state.C[j], expected_C_S3[j]) << "Помилка у змінній C[" << j << "] на стані S(3)";
    }
    EXPECT_EQ(rabbit.state.b, 0) << "Помилка carry-біта на стані S(3)";

    // S(4) 
    rabbit.Strm(dummy_Z);

    uint32_t expected_X_S4[8] = { 0x3B844C36, 0xAF5CD78B, 0x2619A0AC, 0x774FBA88, 0xD16C6AC4, 0x6512AE4E, 0x6A8ECD8F, 0x2BC76513 };
    uint32_t expected_C_S4[8] = { 0x21507597, 0x2869F52D, 0x5FB0832C, 0x68F16B70, 0x888D6952, 0x2B46E1C2, 0x76C2D656, 0xACFB5442 };
    
    for (int j = 0; j < 8; ++j) {
        EXPECT_EQ(rabbit.state.X[j], expected_X_S4[j]) << "Помилка у змінній X[" << j << "] на стані S(4)";
        EXPECT_EQ(rabbit.state.C[j], expected_C_S4[j]) << "Помилка у змінній C[" << j << "] на стані S(4)";
    }
    EXPECT_EQ(rabbit.state.b, 1) << "Помилка carry-біта на стані S(4)";
}