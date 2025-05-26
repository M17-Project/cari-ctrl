#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "interface_cmds.h"
#include "term.h" //colored terminal font

uint8_t rep_buff[1024];
int len;

//config
struct re_config_t
{
    uint64_t rx_freq;
	uint64_t tx_freq;
	float tx_freq_corr;
    float rx_freq_corr;
	float tx_pwr;
	int8_t afc;
	int8_t rx_ena;
    char my_addr[128];
    char re_addr[128];
} config;

//debug printf
void dbg_print(const char* color_code, const char* fmt, ...)
{
	char str[200];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(str, fmt, ap);
	va_end(ap);

	if(color_code!=NULL)
	{
		printf(color_code);
		printf(str);
		printf(TERM_DEFAULT);
	}
	else
	{
		printf(str);
	}
}

int main(int argc, char *argv[])
{
	uint8_t dev_reset = 0;

    if(argc>1)
    {
    	config.tx_pwr=-10.0f;
        config.rx_freq_corr=-10000.0; //normal range is about -50..+50
        config.tx_freq_corr=-10000.0;
    	config.rx_ena=-1;
    	config.afc=-1;
    	
        for(uint8_t i=0; i<argc; i++)
        {
            if(argv[i][0]=='-')
            {
                //single letter args
                if(argv[i][2]==0)
                {
                    //reset
                    if(argv[i][1]=='r')
                    {
						dev_reset = 1;
						i++;
                    }
                    
                    //local PUBlisher's address and port
                    else if(argv[i][1]=='s')
                    {
                        memcpy(config.my_addr, argv[i+1], strlen(argv[i+1]));
                        dbg_print(0, "Local PUB address: ");
                        dbg_print(TERM_GREEN, "%s\n", config.my_addr);
                        i++;
                    }

                    //remote device's address and port
                    else if(argv[i][1]=='d')
                    {
                        memcpy(config.re_addr, argv[i+1], strlen(argv[i+1]));
                        dbg_print(0, "Remote CTRL address: ");
                        dbg_print(TERM_GREEN, "%s\n", config.re_addr);
                        i++;
                    }

                    //RF power
                    else if(argv[i][1]=='p')
                    {
                        config.tx_pwr=atof(argv[i+1]);
                        if(config.tx_pwr<0.0f || config.tx_pwr>47.75f)
                        {
                            dbg_print(TERM_RED, "Invalid TX power\nExiting\n");
                            return 1;
                        }
                        dbg_print(0, "TX power: ");
                        dbg_print(TERM_GREEN, "%2.2f dBm\n", config.tx_pwr);
                        i++;
                    }
                }

                //logner args
                else if(strstr(argv[i], "rf"))
                {
                    config.rx_freq=atoi(argv[i+1]);
                    if(config.rx_freq<420000000U || config.rx_freq>450000000U)
                    {
                        dbg_print(TERM_RED, "Invalid RX frequency\nExiting\n");
                        return 1;
                    }
                    dbg_print(0, "RX frequency: ");
                    dbg_print(TERM_GREEN, "%d Hz\n", config.rx_freq);
                    i++;
                }
                else if(strstr(argv[i], "tf"))
                {
                    config.tx_freq=atoi(argv[i+1]);
                    if(config.tx_freq<420000000U || config.tx_freq>450000000U)
                    {
                        dbg_print(TERM_RED, "Invalid TX frequency\nExiting\n");
                        return 1;
                    }
                    dbg_print(0, "TX frequency: ");
                    dbg_print(TERM_GREEN, "%d Hz\n", config.tx_freq);
                    i++;
                }
                else if(strstr(argv[i], "rc"))
                {
                    config.rx_freq_corr=atof(argv[i+1]);
                    if(config.rx_freq_corr<-100.0f || config.rx_freq_corr>100.0f)
                    {
                        dbg_print(TERM_YELLOW, "RX frequency correction of %3.1fppm seems large\n", config.rx_freq_corr);
                    }
                    dbg_print(0, "RX frequency correction: ");
                    dbg_print(TERM_GREEN, "%3.1f ppm\n", config.rx_freq_corr);
                    i++;
                }
                else if(strstr(argv[i], "tc"))
                {
                    config.tx_freq_corr=atof(argv[i+1]);
                    if(config.tx_freq_corr<-100.0f || config.tx_freq_corr>100.0f)
                    {
                        dbg_print(TERM_YELLOW, "TX frequency correction of %3.1fppm seems large\n", config.tx_freq_corr);
                    }
                    dbg_print(0, "TX frequency correction: ");
                    dbg_print(TERM_GREEN, "%3.1f ppm\n", config.tx_freq_corr);
                    i++;
                }
				else if(strstr(argv[i], "afc"))
                {
                    config.afc=(atoi(argv[i+1])==0)?0:1;
                    dbg_print(0, "AFC: ");
					if(config.afc)
                    	dbg_print(TERM_GREEN, "enabled\n");
					else
						dbg_print(TERM_GREEN, "disabled\n");
                    i++;
                }
                else if(strstr(argv[i], "rx"))
                {
                    if(argv[i+1][0]=='0')
                    {
                    	config.rx_ena=0;
                    }
                    else
                    {
                    	config.rx_ena=1;
					}
                    i++;
                }
            }
        }

        void *zmq_ctx=zmq_ctx_new();
        void *zmq_ctrl=zmq_socket(zmq_ctx, ZMQ_REQ);

        char my_addr[128]={'t', 'c', 'p', ':', '/', '/'}; //this device's address
        char re_addr[128]={'t', 'c', 'p', ':', '/', '/'}; //remote device's address
            
        if(strlen(config.re_addr)>0) //lame validity check
            memcpy(&re_addr[6], config.re_addr, strlen(config.re_addr));
        else
        {
            dbg_print(TERM_YELLOW, "Too short remote device's address\nExiting.");
            return 1;
        }

        dbg_print(0, "ZMQ CTRL ");
        if(zmq_connect(zmq_ctrl, re_addr)==0)
            dbg_print(TERM_GREEN, "connected\n");
        else
        {
            dbg_print(TERM_YELLOW, "fail\nExiting.");
            return 1;
        }

		if(dev_reset)
		{
			dbg_print(0, "Device reset ");

			uint8_t cmd=CMD_DEV_SET_REG;
			uint8_t req[5] = {cmd, 0x05, 0x00, 0x00, 0x00};
			zmq_send(zmq_ctrl, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
			zmq_recv(zmq_ctrl, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
            if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
            {
                if(rep_buff[3]==ERR_OK)
                    dbg_print(TERM_GREEN, "OK\n");
                else
                    dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
            }
            else
            {
                dbg_print(TERM_RED, "- malformed response\n");
            }
			
			return 0;
		}

        //"valid" PUBlisher's address?
        if(strlen(config.my_addr)>0)
        {
            dbg_print(0, "Sending SUB command to device... ");
            memcpy(&my_addr[6], config.my_addr, strlen(config.my_addr));
            uint8_t req[128];
            uint8_t cmd=CMD_SUB_CONN;
            req[0]=cmd;
            memcpy(&req[3], my_addr, strlen(my_addr));
            *((uint16_t*)&req[1])=strlen(my_addr)+3;
            zmq_send(zmq_ctrl, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT); //full address with "tcp://"
            zmq_recv(zmq_ctrl, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
            if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
            {
                if(rep_buff[3]==ERR_OK)
                    dbg_print(TERM_GREEN, "OK\n");
                else
                    dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
            }
            else
            {
                dbg_print(TERM_RED, "malformed response\n");
            }
        }
		
		if(strlen(config.re_addr)>0)
		{
			//TODO: update this
            //rx freq
			/*if(config.rx_freq>0)
			{
				dbg_print(0, "Setting RX freq to %d Hz ", config.rx_freq);
				uint8_t req[128];
				uint8_t cmd=CMD_SUB_SET_PARAM;
				req[0]=cmd;
				memcpy(&req[3], (uint8_t*)&config.rx_freq, sizeof(config.rx_freq));
				*((uint16_t*)&req[1])=sizeof(config.rx_freq)+3;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/

            //tx freq
			/*if(config.tx_freq>0)
			{
				dbg_print(0, "Setting TX freq to %d Hz ", config.tx_freq);
				uint8_t req[128];
				uint8_t cmd=CMD_SET_TX_FREQ;
				req[0]=cmd;
				memcpy(&req[3], (uint8_t*)&config.tx_freq, sizeof(config.tx_freq));
				*((uint16_t*)&req[1])=sizeof(config.tx_freq)+3;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/

            //rx freq correction
			/*if(config.rx_freq_corr>-1000.0f)
			{
				dbg_print(0, "Setting RX freq correction to %3.1f ppm ", config.rx_freq_corr);
				uint8_t req[128];
				uint8_t cmd=CMD_SET_RX_FREQ_CORR;
				req[0]=cmd;
				memcpy(&req[3], (uint8_t*)&config.rx_freq_corr, sizeof(config.rx_freq_corr));
				*((uint16_t*)&req[1])=sizeof(config.rx_freq_corr)+3;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/

            //tx freq correction
			/*if(config.tx_freq_corr>-1000.0f)
			{
				dbg_print(0, "Setting TX freq correction to %3.1f ppm ", config.tx_freq_corr);
				uint8_t req[128];
				uint8_t cmd=CMD_SET_TX_FREQ_CORR;
				req[0]=cmd;
				memcpy(&req[3], (uint8_t*)&config.tx_freq_corr, sizeof(config.tx_freq_corr));
				*((uint16_t*)&req[1])=sizeof(config.tx_freq_corr)+3;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/

			//afc
			/*if(config.afc!=-1)
			{
				if(config.afc)
					dbg_print(0, "Enabling AFC ");
				else
					dbg_print(0, "Disabling AFC ");
				
				uint8_t req[128];
				uint8_t cmd=CMD_SET_AFC;
				req[0]=cmd;
				req[3]=config.afc;
				req[1]=4;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/

            //tx power
			/*if(config.tx_pwr>=0.0f)
			{
                uint8_t pwr_round=floor(config.tx_pwr/0.25f);
				dbg_print(0, "Setting TX power to %2.2f dBm ", pwr_round*0.25f);
				uint8_t req[128];
				uint8_t cmd=CMD_SET_TX_POWER;
				req[0]=cmd;
				req[3]=pwr_round;
				*((uint16_t*)&req[1])=4;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/
			
			//rx enabled?
			/*if(config.rx_ena!=-1)
			{
				dbg_print(0, "RX ");
				if(config.rx_ena==1)
					dbg_print(0, "enable ");
				else
					dbg_print(0, "disable ");
				uint8_t req[128];
				uint8_t cmd=CMD_SET_RX;
				req[0]=cmd;
				req[3]=config.rx_ena;
				*((uint16_t*)&req[1])=4;
				zmq_send(zmq_req, req, *((uint16_t*)&req[1]), ZMQ_DONTWAIT);
				zmq_recv(zmq_req, (char*)rep_buff, sizeof(rep_buff), 0); //get reply
				if(rep_buff[0]==cmd && *((uint16_t*)&rep_buff[1])==4) //response OK?
				{
					if(rep_buff[3]==ERR_OK)
						dbg_print(TERM_GREEN, "OK\n");
					else
						dbg_print(TERM_YELLOW, "ERR %d\n", rep_buff[3]);
				}
				else
				{
					dbg_print(TERM_RED, "malformed response\n");
				}
			}*/
		}

        zmq_close(zmq_ctrl);
        zmq_ctx_destroy(zmq_ctx);
        
        dbg_print(0, "Done, exiting.\n");
        return 0;
    }

    dbg_print(TERM_RED, "Not enough params.\nExiting.\n");
    return 1;
}
