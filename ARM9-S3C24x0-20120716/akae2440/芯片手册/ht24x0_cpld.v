//
//2007.8.25
//zhaoic
//epm3032atc100 for ht2440a
module ht24x0_cpld(	//SEG7
					SEG_A,SEG_B,SEG_C,SEG_D,SEG_E,SEG_F,SEG_G,SEG_DP,
					SEG_G1,SEG_G2,
					nRST,
					//BEEP AND LED
					BEEP,LED,
				   	//EXT
				  	//IDE_ADDR,IDE_INT,IDE_IORDY,IDE_IOR,IDE_IOW,
					EXT_IO1,EXT_IO2,EXT_INT,EXT_CS,
					//KB
					KBOUT,KBIN,
					//SYSTEM
					CLK_10M,CLKOUT0,TOUT0,EINT,
					GPB8,GPB9,GPB10,nGCS1,nGCS2,nGCS4,
					//BUS
					BDATA,BADDR,BADDR23,nWAIT,nOE,nWE,
					//New
					PWM_OUT,SW_PIO,
					
					);

output SEG_A,SEG_B,SEG_C,SEG_D,SEG_E,SEG_F,SEG_G,SEG_DP;
output SEG_G1,SEG_G2;

input nRST;

output BEEP;
output [7:0] LED;
//reversed;
//input [5:1] IDE_ADDR;
//input IDE_INT,IDE_IORDY,IDE_IOR,IDE_IOW;
//reversed;
//EXT BUS
input EXT_IO1,EXT_IO2,EXT_INT,EXT_CS;
output PWM_OUT;
input [3:0] SW_PIO;

input [1:0] KBIN;
output [1:0] KBOUT; 
input CLK_10M,CLKOUT0,TOUT0;
//test
output [2:0] EINT;
input GPB8,GPB9,GPB10;
input nGCS1,nGCS2,nGCS4;
inout [7:0] BDATA;
input [7:0] BADDR;
input BADDR23,nWAIT,nOE,nWE;

reg [7:0] LED;
reg SEG_A,SEG_B,SEG_C,SEG_D,SEG_E,SEG_F,SEG_G,SEG_DP;
reg SEG_G1,SEG_G2;
//reg dout_en,candata_en;
reg [7:0] dat_o,candata_o;

//warning must set gpb8/9/10 to logic 1, audio will work well
//assign GPB8 = 1'b1;
//assign GPB9 = 1'b1;
//assign GPB10 = 1'b1;

assign EINT[0] = KBIN[1];
assign EINT[2] = KBIN[0];
assign KBOUT[0] = GPB9;
assign KBOUT[1] = GPB8;
//////assign BEEP = TOUT0;
//assign BEEP = 1'b0;
assign BEEP = TOUT0 | ~nRST;
assign PWM_OUT = TOUT0 | ~nRST;
//assign EINT[2:0] = 3'b000;




/*
always @(posedge CLKOUT0 or negedge nRST)
	if(!nRST)
	begin
		SEG_G1 = 1'b1;
		SEG_G2 = 1'b1;
		LED[3:0] = 4'b1111;
		candata_en = 0;
	end
	else if(nGCS4 == 0 && nWE == 0 && BADDR23 == 1 && BADDR[7:5] == 3'b000)//0x20800000
	begin
		LED[3:0] = ~BDATA[3:0];
		candata_en = 0;
	end	
	else if(nGCS4 == 0 && nWE == 0 && BADDR23 == 1 && BADDR[7:5] == 3'b100)//0x20800080
	begin
		SEG_G1 = 1'b0;
		SEG_G2 = 1'b0;
		SEG_A = BDATA[0];
		SEG_B = BDATA[1];
		SEG_C = BDATA[2];
		SEG_D = BDATA[3];
		SEG_E = BDATA[4];
		SEG_F = BDATA[5];
		SEG_G = BDATA[6];
		SEG_DP = BDATA[7];
		candata_en = 0;
	end
	else if(nGCS2 == 0 && nWE == 0)
	begin
		candata_en = 1;
		candata_o <= BDATA;	
	end
	else
		candata_en = 0;
*/

always @(posedge nWE or negedge nRST)
	if(!nRST)
	begin
		SEG_G1 = 1'b1;
		SEG_G2 = 1'b1;
		LED[7:0] = 8'haa;
	end
	else if(nGCS4 == 0 && nWE == 1 && BADDR23 == 1 && BADDR[7:5] == 3'b000)//0x20800000
	begin
		LED = ~BDATA;
	end	
	else if(nGCS4 == 0 && nWE == 1 && BADDR23 == 1 && BADDR[7:5] == 3'b100)//0x20800080
	begin
		SEG_G1 = 1'b0;
		SEG_G2 = 1'b0;
		SEG_A = BDATA[0];
		SEG_B = BDATA[1];
		SEG_C = BDATA[2];
		SEG_D = BDATA[3];
		SEG_E = BDATA[4];
		SEG_F = BDATA[5];
		SEG_G = BDATA[6];
		SEG_DP = BDATA[7];
	end

	
	
//assign BDATA = dout_en ? dat_o:8'hzz;	
assign dout_en = (nGCS4 == 0 && nOE == 0 && BADDR23 == 1 )? 1'b1:1'b0;
assign BDATA =  (dout_en && (BADDR[7:5] == 3'b000))? ~LED:										//0x20800000
	  			(dout_en && (BADDR[7:5] == 3'b100))?{SEG_DP,SEG_G,SEG_F,SEG_E,SEG_D,SEG_C,SEG_B,SEG_A}: //0x20800080
	  			(dout_en && (BADDR[7:5] == 3'b101))?{4'h0,SW_PIO}: //0x208000a0
				8'hzz;
	
/*	
always @(posedge CLKOUT0 )//or negedge nRST)
	
	if(nGCS4 == 0 && nOE == 0 && BADDR23 == 1 && BADDR[7:5] == 3'b000)
	begin
		dout_en = 1;
		dat_o <= {4'b0000,~LED[3:0]};
	end
	else if(nGCS4 == 0 && nOE == 0 && BADDR23 == 1 && BADDR[7:5] == 3'b100)
	begin
		dout_en = 1;
		dat_o <= {SEG_DP,SEG_G,SEG_F,SEG_E,SEG_D,SEG_C,SEG_B,SEG_A};
	end
	else if(nGCS2 == 0 && nOE == 0)
	begin
		dout_en = 0;
		dat_o <= CANDATA;
	end
	else
	begin
		dout_en = 0;
	end
*/

endmodule
