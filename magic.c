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

static MagicData rookMagics[64] = {
	{.number = C64(36031551166746720), .mask = C64(282578800148862), .maxIndex = 4095},
	{.number = C64(1154119972415344665), .mask = C64(565157600297596), .maxIndex = 4077},
	{.number = C64(11853483084060041348), .mask = C64(1130315200595066), .maxIndex = 4095},
	{.number = C64(9367489458452303876), .mask = C64(2260630401190006), .maxIndex = 4085},
	{.number = C64(720587279114043424), .mask = C64(4521260802379886), .maxIndex = 4093},
	{.number = C64(6937797429350760453), .mask = C64(9042521604759646), .maxIndex = 4065},
	{.number = C64(2533346739493376), .mask = C64(18085043209519166), .maxIndex = 4048},
	{.number = C64(4755802308200378884), .mask = C64(36170086419038334), .maxIndex = 4095},
	{.number = C64(2886812102790486160), .mask = C64(282578800180736), .maxIndex = 4082},
	{.number = C64(582125439442747432), .mask = C64(565157600328704), .maxIndex = 4094},
	{.number = C64(10957539510103330496), .mask = C64(1130315200625152), .maxIndex = 4089},
	{.number = C64(11259549914769412), .mask = C64(2260630401218048), .maxIndex = 4087},
	{.number = C64(2287533958367232), .mask = C64(4521260802403840), .maxIndex = 4072},
	{.number = C64(281477191303424), .mask = C64(9042521604775424), .maxIndex = 4075},
	{.number = C64(2310381812540638424), .mask = C64(18085043209518592), .maxIndex = 4085},
	{.number = C64(22553200498311424), .mask = C64(36170086419037696), .maxIndex = 4095},
	{.number = C64(2314853232127524880), .mask = C64(282578808340736), .maxIndex = 4095},
	{.number = C64(9367628791358226496), .mask = C64(565157608292864), .maxIndex = 4089},
	{.number = C64(6843364733362764), .mask = C64(1130315208328192), .maxIndex = 4087},
	{.number = C64(4647750085718147088), .mask = C64(2260630408398848), .maxIndex = 3950},
	{.number = C64(2305913962140532993), .mask = C64(4521260808540160), .maxIndex = 4072},
	{.number = C64(37296543590580488), .mask = C64(9042521608822784), .maxIndex = 4094},
	{.number = C64(285880040296456), .mask = C64(18085043209388032), .maxIndex = 4094},
	{.number = C64(35734130262097), .mask = C64(36170086418907136), .maxIndex = 4095},
	{.number = C64(1126521067012128), .mask = C64(282580897300736), .maxIndex = 4095},
	{.number = C64(1247497648687023309), .mask = C64(565159647117824), .maxIndex = 4094},
	{.number = C64(2314920862836327520), .mask = C64(1130317180306432), .maxIndex = 4082},
	{.number = C64(2828844441291195392), .mask = C64(2260632246683648), .maxIndex = 4085},
	{.number = C64(284292542373890), .mask = C64(4521262379438080), .maxIndex = 4047},
	{.number = C64(360428794114772997), .mask = C64(9042522644946944), .maxIndex = 4088},
	{.number = C64(338053663643680912), .mask = C64(18085043175964672), .maxIndex = 4094},
	{.number = C64(140738025242900), .mask = C64(36170086385483776), .maxIndex = 4007},
	{.number = C64(9183123532152896), .mask = C64(283115671060736), .maxIndex = 3987},
	{.number = C64(1478293518291136), .mask = C64(565681586307584), .maxIndex = 4095},
	{.number = C64(317227108663313), .mask = C64(1130822006735872), .maxIndex = 4089},
	{.number = C64(22527104810226176), .mask = C64(2261102847592448), .maxIndex = 3982},
	{.number = C64(648523844167960768), .mask = C64(4521664529305600), .maxIndex = 4056},
	{.number = C64(2305845496142365698), .mask = C64(9042787892731904), .maxIndex = 4092},
	{.number = C64(1143769185400833), .mask = C64(18085034619584512), .maxIndex = 4086},
	{.number = C64(292735222125428864), .mask = C64(36170077829103616), .maxIndex = 4044},
	{.number = C64(9187587883278336), .mask = C64(420017753620736), .maxIndex = 4094},
	{.number = C64(9587749985190928), .mask = C64(699298018886144), .maxIndex = 3876},
	{.number = C64(4701951525577294848), .mask = C64(1260057572672512), .maxIndex = 4095},
	{.number = C64(72343473537159168), .mask = C64(2381576680245248), .maxIndex = 3257},
	{.number = C64(45071464168162308), .mask = C64(4624614895390720), .maxIndex = 4033},
	{.number = C64(1168239526400), .mask = C64(9110691325681664), .maxIndex = 2591},
	{.number = C64(5872695022461158408), .mask = C64(18082844186263552), .maxIndex = 3693},
	{.number = C64(9223374653566877700), .mask = C64(36167887395782656), .maxIndex = 4094},
	{.number = C64(1175721053964468480), .mask = C64(35466950888980736), .maxIndex = 4093},
	{.number = C64(1152930880873087488), .mask = C64(34905104758997504), .maxIndex = 4091},
	{.number = C64(67624380344306720), .mask = C64(34344362452452352), .maxIndex = 4090},
	{.number = C64(9225733813605244992), .mask = C64(33222877839362048), .maxIndex = 4091},
	{.number = C64(36046406938526272), .mask = C64(30979908613181440), .maxIndex = 4094},
	{.number = C64(4526144114139656), .mask = C64(26493970160820224), .maxIndex = 4095},
	{.number = C64(9232380614827573776), .mask = C64(17522093256097792), .maxIndex = 4094},
	{.number = C64(1191488045875003904), .mask = C64(35607136465616896), .maxIndex = 4093},
	{.number = C64(71477139343457), .mask = C64(9079539427579068672), .maxIndex = 4095},
	{.number = C64(54184216619975170), .mask = C64(8935706818303361536), .maxIndex = 4091},
	{.number = C64(5909570470010683661), .mask = C64(8792156787827803136), .maxIndex = 4094},
	{.number = C64(4508074983426086), .mask = C64(8505056726876686336), .maxIndex = 4067},
	{.number = C64(1284316387082241), .mask = C64(7930856604974452736), .maxIndex = 4035},
	{.number = C64(1155177862052024322), .mask = C64(6782456361169985536), .maxIndex = 4095},
	{.number = C64(9066607246868748), .mask = C64(4485655873561051136), .maxIndex = 4093},
	{.number = C64(12249888293830857026), .mask = C64(9115426935197958144), .maxIndex = 4095},
};

static MagicData bishopMagics[64] = {
	{.number = C64(38282256837640224), .mask = C64(18049651735527936), .maxIndex = 1827},
	{.number = C64(1153203807524557584), .mask = C64(70506452091904), .maxIndex = 4084},
	{.number = C64(148909883440300098), .mask = C64(275415828992), .maxIndex = 2925},
	{.number = C64(2305853185067974656), .mask = C64(1075975168), .maxIndex = 2230},
	{.number = C64(13835339581933289506), .mask = C64(38021120), .maxIndex = 1076},
	{.number = C64(612491818138877953), .mask = C64(8657588224), .maxIndex = 3362},
	{.number = C64(432346681189597186), .mask = C64(2216338399232), .maxIndex = 2838},
	{.number = C64(13933700758638592), .mask = C64(567382630219776), .maxIndex = 4043},
	{.number = C64(2314850346479927872), .mask = C64(9024825867763712), .maxIndex = 3550},
	{.number = C64(864695535178743813), .mask = C64(18049651735527424), .maxIndex = 2889},
	{.number = C64(36029348922523776), .mask = C64(70506452221952), .maxIndex = 218},
	{.number = C64(9295434039677160448), .mask = C64(275449643008), .maxIndex = 2675},
	{.number = C64(12682140948994457600), .mask = C64(9733406720), .maxIndex = 3149},
	{.number = C64(577589126178669568), .mask = C64(2216342585344), .maxIndex = 3203},
	{.number = C64(576743636029670400), .mask = C64(567382630203392), .maxIndex = 3091},
	{.number = C64(288285369456168960), .mask = C64(1134765260406784), .maxIndex = 3305},
	{.number = C64(11756647948371886081), .mask = C64(4512412933816832), .maxIndex = 2980},
	{.number = C64(148766200720138288), .mask = C64(9024825867633664), .maxIndex = 3443},
	{.number = C64(144819428628889761), .mask = C64(18049651768822272), .maxIndex = 3218},
	{.number = C64(1297115870421581932), .mask = C64(70515108615168), .maxIndex = 1596},
	{.number = C64(2291408012644368), .mask = C64(2491752130560), .maxIndex = 3953},
	{.number = C64(1161086448511489), .mask = C64(567383701868544), .maxIndex = 4015},
	{.number = C64(596737946011582976), .mask = C64(1134765256220672), .maxIndex = 729},
	{.number = C64(144260323879553056), .mask = C64(2269530512441344), .maxIndex = 3188},
	{.number = C64(153124655222292994), .mask = C64(2256206450263040), .maxIndex = 2719},
	{.number = C64(180425494431598596), .mask = C64(4512412900526080), .maxIndex = 2827},
	{.number = C64(108930851688956432), .mask = C64(9024834391117824), .maxIndex = 3943},
	{.number = C64(1302886369421496322), .mask = C64(18051867805491712), .maxIndex = 4095},
	{.number = C64(149570122178564), .mask = C64(637888545440768), .maxIndex = 4075},
	{.number = C64(1244400912387016712), .mask = C64(1135039602493440), .maxIndex = 3509},
	{.number = C64(1196271469791233), .mask = C64(2269529440784384), .maxIndex = 2895},
	{.number = C64(35201551982624), .mask = C64(4539058881568768), .maxIndex = 305},
	{.number = C64(9800001058460796929), .mask = C64(1128098963916800), .maxIndex = 2584},
	{.number = C64(1152939242822303780), .mask = C64(2256197927833600), .maxIndex = 2722},
	{.number = C64(1741902437164544), .mask = C64(4514594912477184), .maxIndex = 3714},
	{.number = C64(17935917662211), .mask = C64(9592139778506752), .maxIndex = 3915},
	{.number = C64(9808840847474557056), .mask = C64(19184279556981248), .maxIndex = 3965},
	{.number = C64(72660165274419240), .mask = C64(2339762086609920), .maxIndex = 4064},
	{.number = C64(613624247469859840), .mask = C64(4538784537380864), .maxIndex = 4040},
	{.number = C64(2450556342386039296), .mask = C64(9077569074761728), .maxIndex = 3701},
	{.number = C64(3463765168939468928), .mask = C64(562958610993152), .maxIndex = 3977},
	{.number = C64(10161248866888982528), .mask = C64(1125917221986304), .maxIndex = 2903},
	{.number = C64(1152992440295099396), .mask = C64(2814792987328512), .maxIndex = 3549},
	{.number = C64(11637301454573938912), .mask = C64(5629586008178688), .maxIndex = 4047},
	{.number = C64(3458782244326343704), .mask = C64(11259172008099840), .maxIndex = 3968},
	{.number = C64(563089616142601), .mask = C64(22518341868716544), .maxIndex = 3187},
	{.number = C64(70381645856818), .mask = C64(9007336962655232), .maxIndex = 1398},
	{.number = C64(5206453648194543872), .mask = C64(18014673925310464), .maxIndex = 3927},
	{.number = C64(576746694114279424), .mask = C64(2216338399232), .maxIndex = 3870},
	{.number = C64(2305843601952736256), .mask = C64(4432676798464), .maxIndex = 3097},
	{.number = C64(324298760192934209), .mask = C64(11064376819712), .maxIndex = 4077},
	{.number = C64(18016631900872833), .mask = C64(22137335185408), .maxIndex = 314},
	{.number = C64(144397762836865568), .mask = C64(44272556441600), .maxIndex = 1706},
	{.number = C64(2278256812753540), .mask = C64(87995357200384), .maxIndex = 3858},
	{.number = C64(9557207410978979844), .mask = C64(35253226045952), .maxIndex = 3655},
	{.number = C64(4611722303921766416), .mask = C64(70506452091904), .maxIndex = 1002},
	{.number = C64(171137354940090112), .mask = C64(567382630219776), .maxIndex = 1838},
	{.number = C64(2305843043707652292), .mask = C64(1134765260406784), .maxIndex = 2661},
	{.number = C64(7206366338510488640), .mask = C64(2832480465846272), .maxIndex = 1837},
	{.number = C64(12682136550683721737), .mask = C64(5667157807464448), .maxIndex = 660},
	{.number = C64(2314855613691068486), .mask = C64(11333774449049600), .maxIndex = 2236},
	{.number = C64(6926571480054502496), .mask = C64(22526811443298304), .maxIndex = 2282},
	{.number = C64(59690424721211905), .mask = C64(9024825867763712), .maxIndex = 3848},
	{.number = C64(18578483718406401), .mask = C64(18049651735527936), .maxIndex = 4013},
};

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
	u64 x = 0;
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
				"\t{.number = C64(%lu), .mask = C64(%lu), .maxIndex = %d},\n",//table start is set when table is filled
				rookMagics[i].number,
				rookMagics[i].mask,
				rookMagics[i].maxIndex
		);
	}
	printf("};\n\n");
	
	printf("static MagicData bishopMagics[64] = {\n");
	for(int i = 0; i<64; i++){
		printf(
				"\t{.number = C64(%lu), .mask = C64(%lu), .maxIndex = %d},\n",
				bishopMagics[i].number,
				bishopMagics[i].mask,
				bishopMagics[i].maxIndex
		);
	}
	printf("};\n\n");
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
