#ifndef ___INTZIP_BITBUFFER_H___
#define ___INTZIP_BITBUFFER_H___

namespace intzip {
	namespace bits {

	template<typename T, class S>
	struct write_buffer {

		write_buffer(bit_writer<S> &writer, uint8_t bits)
		  : writer(writer), bits(bits)
		{}
		
		virtual ~write_buffer() {}

		void append(const T val)
		{
			// TODO
		}

	protected:
		bit_writer<S> &writer;
		uint8_t bits;
	};

	template<typename T, class S>
	struct buffer_reader {



	protected:
		bit_reader<S> &reader;
	};

	} // namespace bits
} // namespace intzip

#endif

