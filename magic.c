#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "magic.h"
#include "board.h"
#include "print.h"
#include "error.h"

typedef struct MagicData{
	u64 number;
	u64 mask;
	int maxIndex;
	int tableStart;
}MagicData;

static MagicData rookMagics[64];
static MagicData bishopMagics[64];

static u64* rookTable = NULL;

u64 getRookDestinations(int square, u64 occupancy){
	u64 blockers = occupancy & rookMagics[square].mask;
	u64 index = blockers*rookMagics[square].number;
	index >>= 64-12;
	return rookTable[rookMagics[square].tableStart+index];
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

static u64 generateBishopMask(int square){
	u64 mask = 0;
	int directions[8] = { -1,-1, -1,1, 1,-1, 1,1};
	for(int d = 0; d<8; d+=2){
		int dx = directions[d];
		int dy = directions[d+1];
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

static u64 generateRookMask(int square){
	u64 mask = 0;
	int x = square%8;
	int y = square/8;
	mask = (FILE_A<<x) | (RANK_1<<(y*8));
	//reset the square the piece is on and the 4 squares on the edges of the board
	BBReset(mask, boardIndex(x,y));

	BBReset(mask, boardIndex(0,y));
	BBReset(mask, boardIndex(7,y));
	BBReset(mask, boardIndex(x,0));
	BBReset(mask, boardIndex(x,7));
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
static bool tryRookMagic(int square, u64* testTable){
	if(testTable == NULL) error("testTable is null\n");
	u64 magic = u64RandFewbits();
	u64 mask = rookMagics[square].mask;
	int maxIndex = 0;
	for(int i = 0; i<(1<<countBits(mask)); i++){
		u64 blockers = blockersFromMask(mask, i);
		int tableIndex = (blockers*magic)>>(64-12);
		if(testTable[tableIndex] == magic) return false;
		testTable[tableIndex] = magic;
		if(tableIndex>maxIndex) maxIndex = tableIndex;
	}
	rookMagics[square].number = magic;
	rookMagics[square].maxIndex = maxIndex;
	return true;
}

void magicSearch(){
	srandom(time(0));
	//init masks
	for(int i = 0; i<64; i++){
		rookMagics[i].mask = generateRookMask(i);
		bishopMagics[i].mask = generateBishopMask(i);
	}
	
	u64* rookTestTable = malloc(sizeof(u64)*(1<<12));
	u64* bishopTestTable = malloc(sizeof(u64)*(1<<12));
	
	int square = 0;
	while(square<64){
		if(tryRookMagic(square, rookTestTable)){
			printf("Rook magic %d found\n", square);
			square++;
		}
	}
	free(rookTestTable);
	free(bishopTestTable);
}

static u64 genRookDestinationsBitboard(int square, u64 blockers){
	u64 destinations = 0;
	int directions[8] = {1,0, -1,0,  0,1, 0,-1, };
	for(int d = 0; d<8; d+=2){
		int dx = directions[d];
		int dy = directions[d+1];
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

static void genRookTable(){
	int tableSize = 0;
	for(int i = 0; i<64; i++){
		rookMagics[i].tableStart = tableSize;
		tableSize += rookMagics[i].maxIndex+1;
	}
	tableSize *= sizeof(u64);
	rookTable = malloc(tableSize);
	printf("Rook table size %dKb", tableSize/1000);
	for(int i = 0; i<tableSize/sizeof(u64); i++){
		rookTable[i] = (u64)-1;
	}
	for(int square = 0; square<64; square++){
		for(int i = 0; i<(1<<countBits(rookMagics[square].mask)); i++){
			u64 blockers = blockersFromMask(rookMagics[square].mask, i);
			u64 index = blockers*rookMagics[square].number;
			index >>= 64-12;
			rookTable[rookMagics[square].tableStart+index] = genRookDestinationsBitboard(square, blockers);
		}
	}
}

void loadMagics(){
	magicSearch();
	genRookTable();
	int square = boardIndex(0,0);
	
	u64 destinations = genRookDestinationsBitboard(square, blockers);
	printBB(destinations);
	destinations = getRookDestinations(square, blockers);
	printBB(destinations);
}
