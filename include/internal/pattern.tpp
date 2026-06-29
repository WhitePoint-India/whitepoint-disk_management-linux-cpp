

template <std::integral... Bytes>
Pattern Pattern::sequence(Bytes... bytes) {
    return Pattern(std::vector<unsigned char>{ static_cast<unsigned char>(bytes)... });
}
