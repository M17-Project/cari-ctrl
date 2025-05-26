/*
 * interface_cmds.h
 *
 *  Created on: Dec 27, 2023
 *  Revised on: May 26, 2025
 *  
 *      Author: Wojciech Kaczmarski, SP5WWP
 * 				M17 Foundation
 * 
 *   Reference: CARI 1.3
 */
#pragma once

typedef enum
{
    CMD_PING,
    CMD_DEV_SET_REG,
    CMD_SUB_SET_PARAM,
    CMD_SUB_EXEC,
    CMD_SUB_CONN,
    CMD_SUB_START_BB_STREAM,
    CMD_DEV_START_SPVN_STREAM,

    CMD_DEV_GET_IDENT = 0x80,
    CMD_DEV_GET_REG,
    CMD_SUB_GET_CAPS,
    CMD_SUB_GET_PARAM,
    CMD_DEV_GET_SPVN_LIST
} cid_t;

enum cari_err_t
{
	ERR_OK,					//all good
	ERR_MALFORMED,			//malformed frame
	ERR_CMD_UNSUP,			//command unsupported
	ERR_ZMQ_BIND,			//ZMQ port bind failed
	ERR_ZMQ_CONN,			//ZMQ connection failed
	ERR_RANGE				//out of range
};

enum cari_cpbl_t
{
	CAP_AM = 1,
	CAP_FM,
	CAP_SSB,
	CAP_PSK,
	CAP_IQ,
	CAP_DUPLEX = 7
};

