#include "pch.h"
#include "nf/Utility/compression.h"

namespace zlib {
#include "zlib/zlib.h"
}

namespace nf::util {
	bool compress(std::string& in, std::string& out) {
		out.resize(zlib::compressBound(static_cast<unsigned long>(in.length())));

		zlib::Bytef *dest = reinterpret_cast<zlib::Bytef*>(out.data()), *source = reinterpret_cast<zlib::Bytef*>(in.data());
		unsigned long destSize = static_cast<unsigned long>(out.size()), sourceSize = static_cast<unsigned long>(in.size());

		int result = zlib::compress(dest, &destSize, source, sourceSize);
		if (result != Z_OK)
			return false;

		out.resize(destSize);

		return true;
	}

	bool decompress(std::string& in, std::string& out) {
		using namespace zlib;

		zlib::Bytef* source = reinterpret_cast<zlib::Bytef*>(in.data());
		unsigned long destSize = 0, sourceSize = static_cast<unsigned long>(in.size());

		zlib::z_stream stream = {};
		inflateInit(&stream);
		stream.avail_in = sourceSize;
		stream.next_in = source;
		stream.next_out = reinterpret_cast<zlib::Bytef*>(out.data());

		//5 MiB increment
		const int increment = 0x500000;
		int result = 0;
		do {
			size_t oldSize = destSize;
			destSize += increment;
			out.resize(destSize);
			stream.next_out = reinterpret_cast<zlib::Bytef*>(out.data() + oldSize);

			stream.avail_out = increment;

			result = zlib::inflate(&stream, Z_NO_FLUSH);

		} while (result == Z_BUF_ERROR);

		if (result != Z_OK)
			return false;

		zlib::inflateEnd(&stream);

		out.resize(stream.total_out);

		return true;
	}
}