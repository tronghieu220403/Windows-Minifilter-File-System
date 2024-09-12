#ifndef KMPMATCHER_H
#define KMPMATCHER_H

template <typename T>
class KMPMatcher {
private:
    T* text;
    size_t text_size;
    T* pattern;
    size_t pattern_size;
    size_t* lps;

    void ComputeLpsArray();

public:
    KMPMatcher(T* text, size_t text_size, T* pattern, size_t pattern_size);
    ~KMPMatcher();
    size_t KmpSearch();
};

template <typename T>
inline KMPMatcher<T>::KMPMatcher(T* text, size_t text_size, T* pattern, size_t pattern_size)
    : text(text), text_size(text_size), pattern(pattern), pattern_size(pattern_size) {
    lps = new size_t[pattern_size];
    ComputeLpsArray();
}

template <typename T>
inline KMPMatcher<T>::~KMPMatcher() {
    delete[] lps;
}

template <typename T>
inline void KMPMatcher<T>::ComputeLpsArray() {
    size_t length = 0;
    size_t i = 1;
    lps[0] = 0;

    while (i < pattern_size) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        }
        else {
            if (length != 0) {
                length = lps[length - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

template <typename T>
inline size_t KMPMatcher<T>::KmpSearch() {
    size_t i = 0; // index for text
    size_t j = 0; // index for pattern

    while (i < text_size) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == pattern_size) {
            return i - j; // Found pattern at index (i - j)
        }
        else if (i < text_size && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }

    return static_cast<size_t>(-1); // Pattern not found
}


#endif // KMPMATCHER_H
