#pragma once

#include "SceneSerializer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

namespace AppKit
{
    namespace GLEngine
    {

        typedef rapidjson::Writer<rapidjson::StringBuffer> RapidJsonWriter;

        struct WriterSet {
            rapidjson::StringBuffer stringBuffer;
            RapidJsonWriter writer;
            WriterSet():writer(stringBuffer){}
        };

        class JSONSceneSerializer
        {
        public:

            static std::shared_ptr<WriterSet> Begin();
            static void Serialize(RapidJsonWriter &writer, std::shared_ptr<Transform> transform, bool include_root);
            static Platform::ObjectBuffer End(std::shared_ptr<WriterSet> writerSet);

            // Platform::ObjectBuffer serialize(std::shared_ptr<Transform> transform, bool include_root);
            // std::shared_ptr<Transform> deserialize(const Platform::ObjectBuffer &src);
        };

        struct ReaderSet {
            Platform::ObjectBuffer objectBuffer;
            rapidjson::Document document;
        };

        class JSONSceneDeserializer
        {
        public:

            static std::shared_ptr<ReaderSet> Begin(Platform::ObjectBuffer *src);
            static void Deserialize(rapidjson::Value &reader, bool include_root, std::shared_ptr<Transform> target_root);
        };

    }
}
