//
// Created by andyroiiid on 12/22/2022.
//

#pragma once

#include <string>

class HdrImageFile {
public:
    HdrImageFile() = default;

    explicit HdrImageFile(const std::string &filename);

    ~HdrImageFile() {
        Release();
    }

    HdrImageFile(const HdrImageFile &) = delete;

    HdrImageFile &operator=(const HdrImageFile &) = delete;

    HdrImageFile(HdrImageFile &&other) noexcept {
        Swap(other);
    }

    HdrImageFile &operator=(HdrImageFile &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(HdrImageFile &other) noexcept;

    [[nodiscard]] uint32_t GetWidth() const { return m_width; }

    [[nodiscard]] uint32_t GetHeight() const { return m_height; }

    [[nodiscard]] const float *GetData() const { return m_data; }

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    float *m_data = nullptr;
};
