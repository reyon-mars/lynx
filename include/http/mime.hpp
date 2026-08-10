#pragma once
#include <unordered_map>
#include <string_view>

namespace http {

    inline std::string_view get_mime_type( std::string_view extension )
    {
        static const std::unordered_map<std::string_view, std::string_view> mime_types 
        {
            // Text
            {".html",   "text/html"},
            {".htm",    "text/html"},
            {".css",    "text/css"},
            {".csv",    "text/csv"},
            {".txt",    "text/plain"},
            {".xml",    "application/xml"},

            // JavaScript / JSON
            {".js",     "text/javascript"},
            {".mjs",    "text/javascript"},
            {".json",   "application/json"},

            //Images
            {".png",    "image/png"},
            {".jpg",    "image/jpeg"},
            {".jpeg",   "image/jpeg"},
            {".gif",    "image/gif"},
            {".svg",    "image/svg+xml"},
            {".webp",   "image/webp"},
            {".ico",    "iamge/x-icon"},
            
            // Audio
            {".mp3",    "audio/mpeg"},
            {".wav",    "audio/wav"},
            {".ogg",    "audio/ogg"},

            // Video
            {".mp4",    "video/mp4"},
            {".webm",   "video/webm"},
            {".avi",    "video/x-msvideo"},

            // Documents / archives
            {".pdf",    "application/pdf"},
            {".zip",    "application/zip"},
            {".gz",     "application/gzip"},

            // Fonts
            {".woff",   "font/woff"},
            {".woff2",  "font/woff2"},
            {".ttf",    "font/ttf"},
            {".otf",    "font/otf"}
        };
        
        auto it{ mime_types.find(extension)};
        
        if( it != mime_types.end())
        {
            return it->second;
        }
        return "application/octet-stream";
    }
}