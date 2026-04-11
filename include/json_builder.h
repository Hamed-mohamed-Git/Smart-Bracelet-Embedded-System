#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <Arduino.h>

class JsonBuilder {
public:
    JsonBuilder() : count(0) {
        buffer = "{";
    }

    JsonBuilder& add(const char* key, int32_t value) {
        appendSeparator();
        buffer += "\"";
        buffer += key;
        buffer += "\":";
        buffer += String(value);
        return *this;
    }

    JsonBuilder& add(const char* key, const char* value) {
        appendSeparator();
        buffer += "\"";
        buffer += key;
        buffer += "\":\"";
        buffer += value;
        buffer += "\"";
        return *this;
    }

    String build() const {
        return buffer + "}";
    }

    const char* c_str() {
        result = build();
        return result.c_str();
    }

protected:
    String buffer;
    String result;
    int count;

    void appendSeparator() {
        if (count > 0) buffer += ",";
        count++;
    }
};

#endif
