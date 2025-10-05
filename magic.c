#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "magic.h"
#include "board.h"

typedef struct MagicData{
	u64 number;
	u64 mask;
	int maxIndex;
	int tableStart;
}MagicData;

static MagicData rookMagics[64];
static MagicData bishopMagics[64];

static u64* rookTable = NULL;
static u64* bishopTable = NULL;

u64 getRookDestinations(int square, u64 occupancy){
	u64 blockers = occupancy & rookMagics[square].mask;
	u64 index = blockers*rookMagics[square].number;
	index >>= 64-12;
	return rookTable[rookMagics[square].tableStart+index];
}

u64 getBishopDestinations(int square, u64 occupancy){
	u64 blockers = occupancy & bishopMagics[square].mask;
	u64 index = blockers*bishopMagics[square].number;
	index >>= 64-12;
	return bishopTable[bishopMagics[square].tableStart+index];
}

static u64 u64Rand(){
	u64 x = (u64)0;
	x |= (random()&(0xFFFF));
	x |= (random()&(0xFFFF))<<16;
	x |= (random()&(0xFFFF))<<32;
	x |= (random()&(0xFFFF))<<48;
	return x;
};

static u64 u64RandFewbits(){
	return u64Rand()&u64Rand()&u64Rand();
}

static u64 attackMask(int square, bool bishop){
	u64 mask = 0;
	const int bishopDirections[8] =  { -1,-1, -1,1, 1,-1, 1,1};
	const int rookDirections[8] =  { 1 ,0, -1,0, 0,1, 0,-1};
	for(int d = 0; d<8; d+=2){
		int dx = bishop ? bishopDirections[d] : rookDirections[d];
		int dy = bishop ? bishopDirections[d+1] : rookDirections[d+1];
		int x = square%8;
		int y = square/8;
		for(int i = 0; i<8; i++){
			x+=dx;
			y+=dy;
			if((x+dx)<0 || (x+dx)>=8 || (y+dy)<0 || (y+dy)>=8)
				break;
			BBSet(mask, boardIndex(x,y));
		}
	}
	return mask;
}

static u64 blockersFromMask(u64 mask, int x){//x should be in the range 0-2^(num bits in mask)
	u64 blockers = 0;
	int i = 0;
	while(mask){
		if(x&(1<<i))
			BBSet(blockers, bitScanForward(mask));
		mask &= mask-1;
		i++;
	}
	return blockers;
}

//return true if a valid magic was found that was better than the last one
static bool tryMagic(int square, u64* testTable, bool bishop){
	u64 magic = u64RandFewbits();
	u64 mask = bishop ? bishopMagics[square].mask : rookMagics[square].mask;
	int maxIndex = 0;
	for(int i = 0; i<(1<<countBits(mask)); i++){
		u64 blockers = blockersFromMask(mask, i);
		int tableIndex = (blockers*magic)>>(64-12);
		if(testTable[tableIndex] == magic) return false;
		testTable[tableIndex] = magic;
		if(tableIndex>maxIndex) maxIndex = tableIndex;
	}
	if(bishop){
		bishopMagics[square].number = magic;
		bishopMagics[square].maxIndex = maxIndex;
	}else{
		rookMagics[square].number = magic;
		rookMagics[square].maxIndex = maxIndex;
	}
	return true;
}

static void printMagics(){
	printf("static MagicData rookMagics[64] = {\n");
	for(int i = 0; i<64; i++){
		printf(
				"\t{.number = (u64)%lu, .mask = (u64)%lu, .maxIndex = %d},\n",//table start is set when table is filled
				rookMagics[i].number,
				rookMagics[i].mask,
				rookMagics[i].maxIndex
		);
	}

	printf("static MagicData bishopMagics[64] = {\n");
	for(int i = 0; i<64; i++){
		printf(
				"\t{.number = (u64)%lu, .mask = (u64)%lu, .maxIndex = %d},\n",
				bishopMagics[i].number,
				bishopMagics[i].mask,
				bishopMagics[i].maxIndex
		);
	}

	printf("};\n");
}

void magicSearch(){
	srandom(time(0));
	//init masks
	for(int i = 0; i<64; i++){
		rookMagics[i].mask = attackMask(i, false);
		bishopMagics[i].mask = attackMask(i, true);
	}
	
	u64* testTable = malloc(sizeof(u64)*(1<<12));
	
	int square = 0;
	while(square<64){
		if(tryMagic(square, testTable, false))
			square++;
	}
	
	square = 0;
	while(square<64){
		if(tryMagic(square, testTable, true))
			square++;
	}
	free(testTable);
	printMagics();
}

static u64 destinationsBitboard(int square, u64 blockers, bool bishop){
	u64 destinations = 0;
	const int bishopDirections[8] =  { -1,-1, -1,1, 1,-1, 1,1};
	const int rookDirections[8] =  { 1 ,0, -1,0, 0,1, 0,-1};
	for(int d = 0; d<8; d+=2){
		int dx = bishop ? bishopDirections[d] : rookDirections[d];
		int dy = bishop ? bishopDirections[d+1] : rookDirections[d+1];
		int x = square%8;
		int y = square/8;
		for(int i = 0; i<8; i++){
			x+=dx;
			y+=dy;
			if( (x)<0 || (x)>=8 || (y)<0 || (y)>=8)	
				break;
			BBSet(destinations, boardIndex(x,y));
			if(BBGet(blockers, boardIndex(x,y)))
				break;
		}
	}
	return destinations;
}

static void fillRookTable(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		rookMagics[i].tableStart = tableSize;
		tableSize += rookMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	rookTable = malloc(tableSize);
	printf("Rook table size %dKb\n", tableSize/1000);

	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<countBits(rookMagics[square].mask)); i++){
			u64 blockers = blockersFromMask(rookMagics[square].mask, i);
			u64 index = blockers*rookMagics[square].number;
			index >>= 64-12;
			rookTable[rookMagics[square].tableStart+index] = destinationsBitboard(square, blockers, false);
		}
	}
}

static void fillBishopTable(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		bishopMagics[i].tableStart = tableSize;
		tableSize += bishopMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	bishopTable = malloc(tableSize);
	printf("Bishop table size %dKb\n", tableSize/1000);

	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<countBits(bishopMagics[square].mask)); i++){
			u64 blockers = blockersFromMask(bishopMagics[square].mask, i);
			u64 index = blockers*bishopMagics[square].number;
			index >>= 64-12;
			bishopTable[bishopMagics[square].tableStart+index] = destinationsBitboard(square, blockers, true);
		}
	}
}

void fillMagicTables(){
	fillRookTable();
	fillBishopTable();
}
