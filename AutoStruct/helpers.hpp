#pragma once

enum ARG_FLAGS
{
	INVALID_ARGUMENT = 0x00,
	DEFAULT          = 0x01,
	ALIGN_EQUAL_SIGN = 0x02,
	ALIGN_VALUES     = 0x04,
	HEX_VALUES       = 0x08,
	ALIGN_NUM_32_BIT = 0x10,
	ALIGN_NUM_64_BIT = 0x20,
	IDA_TO_CPP       = 0x40,
	CPP_TO_IDA       = 0x80,
};