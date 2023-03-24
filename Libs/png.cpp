
#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <algorithm>

#include <Tools.h>


#include "png.h"

/*
I don't remember how and when I built this, but... I remember migrating it from C++ to C and now... I'm "migrating" back to C++... 
so I'm going to need some help rewriting this frankenstein...
*/


#define MAX_NUM(a, b)                       (((a) > (b)) ? (a) : (b))
#define READBIT(pstBitPtr, pbBitStream)     ((pbBitStream[pstBitPtr >> 3] >> (pstBitPtr & 0x7)) & (unsigned char)1)


typedef enum PngColorType
{
    LCT_GREY = 0,           /*Greyscale: 1,2,4,8,16 bit*/
    LCT_RGB = 2,            /*RGB: 8,16 bit*/
    LCT_PALETTE = 3,        /*Palette: 1,2,4,8 bit*/
    LCT_GREY_ALPHA = 4,     /*Greyscale with alpha: 8,16 bit*/
    LCT_RGBA = 6            /*RGB with alpha: 8,16 bit*/
}PngColorType;


typedef struct PngDecoderSettings_t
{
    unsigned char  bIgnoreAdler32;     /*si es 1 continua decodificando la imange aunque el crc Adler32 este mal*/
    unsigned char  bIgnoreCrc;         /*ignorar checksums*/
    unsigned char  bIgnoreCritical;    /*ignorar error en algun chunk*/
    unsigned char  bIgnoreEnd;         /*ignorar problemas con algun chunk u otra cosa.. al final del archivo */
    unsigned char  bColorConvert;      /*Si desea convertir el tipo PNG al color otro color */
    unsigned char  bReadTextChunks;    /*leer los textos chunks.. si*/
} PngDecoderSettings_t;

/*
    Tipo de color, paleta TODO: probar bien implementacion sayanica de la paleta
    Transparencias... bits
*/
typedef struct PngColorMode_t
{
    PngColorType        ColorType;
    unsigned char       bBitdepth;
    unsigned char       *pbPalette;
    size_t              stPaletteSize;
    unsigned char       bKeyDefined; /*Si tiene color de transparecia, abajo define RGB*/
    unsigned short      ulKeyR;
    unsigned short      ulKeyG;
    unsigned short      ulKeyB;
}PngColorMode_t;


/*
    IHDR :Header
    PLTE :Palette
    Transparency :chunk tRNS
 */
typedef struct PngInfo_t
{

    unsigned char                     bCompressionMethod;     /*Metodo de compresion (byte 26) siempre 0 */
    unsigned char                     bFilterMethod;          /*Tipo de filtro utilizado originalmente */
    unsigned char                     bInterlaceMethod;       /*Metodo original de entrelazado: 0=sin, 1=Adam7 */
    PngColorMode_t                    Color;
    unsigned char                     bBackgroundDefined; /*Color de background sugerido en el chunk bKGD, debajo define rgb */
    unsigned short                    ulBackgroundR;
    unsigned short                    ulBackgroundG;
    unsigned short                    ulBackgroundB;


    /*chunks tEXtand zTXt "no internacionales" */

    size_t      stTextNum;          /* Total de psTextStrings*/
    char         **psTextKeys;       /* Keyword del chunk*/
    char         **psTextStrings;

    /*
    * chunk iTXt "internacional"
    */

    size_t      stiTextNum;
    char        **psiTextKeys;
    char        **psiTextLangTags;      /*ISO 639 language tag*/
    char        **psiTextTranskeys;     /*Keyword traducido - UTF-8*/
    char        **psiTextStrings;       /*Text internacional actual - UTF-8*/
}PngInfo_t;

typedef struct PngState_t
{
    PngDecoderSettings_t     Decoder;    /*seteos para decodificar*/
    PngColorMode_t           InfoRaw;    /*formato en el cual deberia tomar los pixles en crudo*/
    PngInfo_t                Info;       /*info del PNG despues de la decodificacion */
    unsigned char            bError;
}PngState_t;



unsigned char pngReadBitFromReversedStream( size_t* pstBitPtr, const unsigned char* pbBitStream )
{
    unsigned char bRet = (unsigned char)( ( pbBitStream[ ( *pstBitPtr ) >> 3 ] >> ( 7 - ( ( *pstBitPtr ) & 0x7 ) ) ) & 1 );
    ++( *pstBitPtr );
    return bRet;
}

unsigned char pngGetNumColorChannels( PngColorType ColorType )
{
    switch ( ColorType )
    {
        case LCT_GREY:
            return 1;
        case LCT_RGB:
            return 3;
        case LCT_PALETTE:
            return 1;
        case LCT_GREY_ALPHA:
            return 2;
        case LCT_RGBA:
            return 4;
    }

    return 0;
}

/*
            ***** ADAM7 ****
            https://en.wikipedia.org/wiki/Adam7_algorithm
*/

static const unsigned char ADAM7_IX[ 7 ] = { 0, 4, 0, 2, 0, 1, 0 }; //x start values
static const unsigned char ADAM7_IY[ 7 ] = { 0, 0, 4, 0, 2, 0, 1 }; //y start values
static const unsigned char ADAM7_DX[ 7 ] = { 8, 8, 4, 4, 2, 2, 1 }; //x delta values
static const unsigned char ADAM7_DY[ 7 ] = { 8, 8, 8, 4, 4, 2, 2 }; //y delta values*/

/*
* Adam7GetPassValues
    Los valores de stPassStart tienen 8 valores: el octavo indica el byte después del final de la séptima pasada
*/
void pngAdam7GetPassValues(
    unsigned passw[ 7 ], unsigned passh[ 7 ], size_t stFilterPassStart[ 8 ],
    size_t stPaddedPassStart[ 8 ], size_t stPassStart[ 8 ],
    unsigned w, unsigned h, unsigned bpp
)
{
    unsigned i;

    //calcular width and height en pixels por cada pasada
    for ( i = 0; i != 7; ++i )
    {
        passw[ i ] = ( w + ADAM7_DX[ i ] - ADAM7_IX[ i ] - 1 ) / ADAM7_DX[ i ];
        passh[ i ] = ( h + ADAM7_DY[ i ] - ADAM7_IY[ i ] - 1 ) / ADAM7_DY[ i ];
        if ( passw[ i ] == 0 )
            passh[ i ] = 0;
        if ( passh[ i ] == 0 )
            passw[ i ] = 0;
    }

    stFilterPassStart[ 0 ] = stPaddedPassStart[ 0 ] = stPassStart[ 0 ] = 0;
    for ( i = 0; i != 7; ++i )
    {
        //si passw[i] el 0 = no filtertype-byte
        stFilterPassStart[ i + 1 ] = stFilterPassStart[ i ] + ( ( passw[ i ] && passh[ i ] ) ? passh[ i ] * ( 1 + ( passw[ i ] * bpp + 7 ) / 8 ) : 0 );
        //llenar el byte completo al final de cada linea
        stPaddedPassStart[ i + 1 ] = stPaddedPassStart[ i ] + passh[ i ] * ( ( passw[ i ] * bpp + 7 ) / 8 );
        //solo padded al final de la imagen reducida
        stPassStart[ i + 1 ] = stPassStart[ i ] + ( passh[ i ] * passw[ i ] * bpp + 7 ) / 8;
    }
}

void pngAdam7SetBitOfReversedStream( size_t *pstBitPointer, unsigned char *pbBitStream, unsigned char bBit )
{
    if ( bBit )
        pbBitStream[ ( *pstBitPointer ) >> 3 ] |= (unsigned char)( bBit << ( 7 - ( ( *pstBitPointer ) & 0x7 ) ) );
    ++( *pstBitPointer );
}


void pngAdam7Deinterlace( unsigned char *pbOut, const unsigned char *pbyMem, unsigned w, unsigned h, unsigned bpp )
{
    unsigned passw[ 7 ], passh[ 7 ];
    size_t  stFilterPassStart[ 8 ];
    size_t  stPaddedPassStart[ 8 ];
    size_t  stPassStart[ 8 ];
    unsigned i;

    pngAdam7GetPassValues( passw, passh, stFilterPassStart, stPaddedPassStart, stPassStart, w, h, bpp );

    if ( bpp >= 8 )
    {
        for ( i = 0; i != 7; ++i )
        {
            unsigned x, y, b;
            size_t bytewidth = bpp / 8;
            for ( y = 0; y < passh[ i ]; ++y )
                for ( x = 0; x < passw[ i ]; ++x )
                {
                    size_t pixelinstart = stPassStart[ i ] + ( y * passw[ i ] + x ) * bytewidth;
                    size_t pixeloutstart = ( ( ADAM7_IY[ i ] + y * ADAM7_DY[ i ] ) * w + ADAM7_IX[ i ] + x * ADAM7_DX[ i ] ) * bytewidth;
                    for ( b = 0; b < bytewidth; ++b )
                        pbOut[ pixeloutstart + b ] = pbyMem[ pixelinstart + b ];
                }
        }
    }
    else
    {
        for ( i = 0; i != 7; ++i )
        {
            unsigned x, y, b;
            unsigned ilinebits = bpp * passw[ i ];
            unsigned olinebits = bpp * w;
            size_t obp, ibp; /*bit pointers (for out and in buffer)*/
            for ( y = 0; y < passh[ i ]; ++y )
                for ( x = 0; x < passw[ i ]; ++x )
                {
                    ibp = ( 8 * stPassStart[ i ] ) + ( y * ilinebits + x * bpp );
                    obp = ( ADAM7_IY[ i ] + y * ADAM7_DY[ i ] ) * olinebits + ( ADAM7_IX[ i ] + x * ADAM7_DX[ i ] ) * bpp;
                    for ( b = 0; b < bpp; ++b )
                    {
                        unsigned char bit = pngReadBitFromReversedStream( &ibp, pbyMem );
                        pngAdam7SetBitOfReversedStream( &obp, pbOut, bit );
                    }
                }
        }
    }
}


/*

        *****************
*/

/*
static unsigned long toUint32( const unsigned char *pb )
{
    return (unsigned long)( ( pb[ 0 ] << 24 ) | ( pb[ 1 ] << 16 ) | ( pb[ 2 ] << 8 ) | pb[ 3 ] );
}
*/

unsigned char pngIsChunk( const unsigned char *pbChunk, const char *sType )
{
    if ( 4 != strlen( sType ) )
        return 0;

    return ( pbChunk[ 4 ] == sType[ 0 ] && pbChunk[ 5 ] == sType[ 1 ] && pbChunk[ 6 ] == sType[ 2 ] && pbChunk[ 7 ] == sType[ 3 ] );
}


unsigned char pngReadBitFromStream( size_t* pstBitPtr, const unsigned char* pbBitStream )
{
    unsigned char bRet = (unsigned char)( READBIT( *pstBitPtr, pbBitStream ) );
    ++( *pstBitPtr );
    return bRet;
}

size_t pngReadBitsFromStream( size_t* pstBitPtr, const unsigned char* pbBitStream, size_t stBits )
{
    size_t stResult = 0, i;
    for ( i = 0; i != stBits; ++i )
    {
        stResult += ( (unsigned)READBIT( *pstBitPtr, pbBitStream ) ) << i;
        ++( *pstBitPtr );
    }
    return stResult;
}

unsigned char pngGetBpp( const PngColorMode_t* pInfo )
{
    //calcular bits por pixel fuera del tipo de color y profundidad
    return (unsigned char)(pngGetNumColorChannels( pInfo->ColorType ) * pInfo->bBitdepth );
}


/*

            ************ Huffman ********
            https://www.programiz.com/dsa/huffman-coding
*/

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285
/*256 literals, the end code, some length codes, and 2 unused codes*/
#define NUM_DEFLATE_CODE_SYMBOLS 288
/*the distance codes have their own symbols, 30 used, 2 unused*/
#define NUM_DISTANCE_SYMBOLS 32
/*the code length codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros*/
#define NUM_CODE_LENGTH_CODES 19


static const unsigned LENGTHBASE[ 29 ] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
static const unsigned LENGTHEXTRA[ 29 ] = { 0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3, 4,  4,  4,   4,   5,   5,   5,   5,   0 };
static const unsigned CLCL_ORDER[ NUM_CODE_LENGTH_CODES ] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
static const unsigned DISTANCEBASE[ 30 ] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
static const unsigned DISTANCEEXTRA[ 30 ] = { 0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,   6,   6,   7,   7,   8, 8,    9,    9,   10,   10,   11,   11,   12,    12,    13,    13 };


typedef struct PngHuffmanTree
{
    unsigned    *punTree2D;
    unsigned    *punTree1D;
    unsigned    *punLengths;
    unsigned    unMaxBitLen;
    unsigned    unNumCodes;
}PngHuffmanTree;


void pngHuffmanTreeInit(PngHuffmanTree*pTree )
{
    pTree->punTree2D = nullptr;
    pTree->punTree1D = nullptr;
    pTree->punLengths = nullptr;
}

void pngHuffmanTreeCleanup(PngHuffmanTree*pTree )
{
    if ( pTree->punTree2D )
        free( pTree->punTree2D );

    if ( pTree->punTree1D )
        free( pTree->punTree1D );

    if ( pTree->punLengths )
        free( pTree->punLengths );
}

unsigned pngHuffmanDecodeSymbol( const unsigned char* pbMem, size_t *punBitPtr, const PngHuffmanTree*pTreeCodes, size_t stMemBitLength )
{
    unsigned unTreePos = 0;
    unsigned unCt;

    for ( ;;)
    {
        if ( *punBitPtr >= stMemBitLength )
            return (unsigned)( -1 ); //error: end of input memory reached without endcode

        //decode the symbol from the tree. The "readBitFromStream" code is inlined in
        //the expression below because this is the biggest bottleneck while decoding

        unCt = pTreeCodes->punTree2D[ ( unTreePos << 1 ) + READBIT( *punBitPtr, pbMem ) ];
        ++( *punBitPtr );
        if ( unCt < pTreeCodes->unNumCodes )
            return unCt; //the symbol is decoded, return it
        else
            unTreePos = unCt - pTreeCodes->unNumCodes; //symbol not yet decoded, instead move tree position

        if ( unTreePos >= pTreeCodes->unNumCodes )
            return (unsigned)( -1 ); //error: it appeared outside the codetree
    }
}


unsigned char pngHuffmanTreeMake2DTree(PngHuffmanTree*pTree )
{
    unsigned long udNodeFilled = 0;
    unsigned long udTreePos = 0;
    unsigned long n, i;

    if (nullptr != ( pTree->punTree2D = (unsigned*)malloc( pTree->unNumCodes * 2 * sizeof( unsigned ) ) ) )
    {
        for ( n = 0; n < pTree->unNumCodes * 2; ++n )
            pTree->punTree2D[ n ] = 32767;

        for ( n = 0; n < pTree->unNumCodes; ++n )
        {
            for ( i = 0; i != pTree->punLengths[ n ]; ++i )
            {
                unsigned char bit = (unsigned char)( ( pTree->punTree1D[ n ] >> ( pTree->punLengths[ n ] - i - 1 ) ) & 1 );

                if ( udTreePos > 2147483647 || udTreePos + 2 > pTree->unNumCodes )
                    return 55;

                if ( 32767 == pTree->punTree2D[ 2 * udTreePos + bit ] )
                {
                    if ( i + 1 == pTree->punLengths[ n ] )
                    {
                        pTree->punTree2D[ 2 * udTreePos + bit ] = n;
                        udTreePos = 0;
                    }
                    else
                    {
                        ++udNodeFilled;
                        pTree->punTree2D[ 2 * udTreePos + bit ] = udNodeFilled + pTree->unNumCodes;
                        udTreePos = udNodeFilled;
                    }
                }
                else
                    udTreePos = pTree->punTree2D[ 2 * udTreePos + bit ] - pTree->unNumCodes;
            }
        }

        for ( n = 0; n < pTree->unNumCodes * 2; ++n )
        {
            if ( 32767 == pTree->punTree2D[ n ] )
                pTree->punTree2D[ n ] = 0;
        }

        return 0;
    }

    return 1;
}

//template<typename T>
//void pngVectorResizeAndFill(std::vector<T> &vct, size_t stSize)
//{
//    vct.resize(stSize);
//    std::fill(vct.begin(), vct.end(), 0);
//}

unsigned char pngHuffmanTreeMakeFromLengths1D( PngHuffmanTree *pTree )
{

    unsigned char bRet = 1;
    unsigned bits, n;

    std::vector<unsigned long> vctBitLenCount;
    std::vector<unsigned long> vctNextCode;

    if (nullptr != ( pTree->punTree1D = (unsigned*)malloc( pTree->unNumCodes * sizeof( unsigned ) ) ) )
    {

        vctBitLenCount.resize((size_t)pTree->unMaxBitLen + 1);
        vctNextCode.resize(pTree->unMaxBitLen + 1 );

        //contar el numero de instancas por cada lenth code 
        for ( bits = 0; bits < pTree->unNumCodes; bits++ )
            ++vctBitLenCount[pTree->punLengths[bits]];

        //valores nextcode 
        for ( bits = 1; bits <= pTree->unMaxBitLen; ++bits )
            vctNextCode[ bits ] = ( vctNextCode[ bits - 1 ] + vctBitLenCount[ bits - 1 ] ) << 1;

        //generar todos los codigos
        for ( n = 0; n < pTree->unNumCodes; n++ )
        {
            if ( 0 != pTree->punLengths[ n ] )
                pTree->punTree1D[ n ] = vctNextCode[ pTree->punLengths[ n ] ]++;
        }

        bRet = pngHuffmanTreeMake2DTree( pTree );
        

    }

    return bRet;
}

unsigned char pngHuffmanTreeMakeFromLengths( PngHuffmanTree *pTree, const unsigned *punBitLen, size_t stNumCodes, unsigned unMaxBitLen )
{
    pTree->punLengths = (unsigned*)malloc( stNumCodes * sizeof( unsigned ) );

    if ( pTree->punLengths )
    {
        for ( unsigned i = 0; i < stNumCodes; i++ )
            pTree->punLengths[ i ] = punBitLen[ i ];

        pTree->unNumCodes = (unsigned)stNumCodes;
        pTree->unMaxBitLen = unMaxBitLen;

        return pngHuffmanTreeMakeFromLengths1D( pTree );
    }

    return 1;
}

unsigned char pngGenerateFixedDistanceTree( PngHuffmanTree *pTreeDist )
{
    unsigned char bRet = 1;
    unsigned *unpBitLen = (unsigned*)malloc( NUM_DISTANCE_SYMBOLS * sizeof( unsigned ) );

    if ( unpBitLen )
    {
        //hay 32 codigos de distancias pero 30-31 no se usan
        for ( unsigned char i = 0; i < NUM_DISTANCE_SYMBOLS; i++ )
            unpBitLen[ i ] = 5;

        bRet = pngHuffmanTreeMakeFromLengths( pTreeDist, unpBitLen, NUM_DISTANCE_SYMBOLS, 15 );

        free( unpBitLen );
    }

    return bRet;
}

unsigned char pngGenerateFixedBitLenTree( PngHuffmanTree *pTree )
{

    unsigned char  bRet = 1;
    unsigned *punBitLen = (unsigned*)malloc( NUM_DEFLATE_CODE_SYMBOLS * sizeof( unsigned ) );

    if ( punBitLen )
    {
        /*
           288 possible codes:
           0-255=literals
           256=endcode
           257-285=lengthcodes
           286-287=unused
        */
        unsigned i;
        for ( i = 0; i <= 143; ++i )
            punBitLen[ i ] = 8;
        for ( i = 144; i <= 255; ++i )
            punBitLen[ i ] = 9;
        for ( i = 256; i <= 279; ++i )
            punBitLen[ i ] = 7;
        for ( i = 280; i <= 287; ++i )
            punBitLen[ i ] = 8;

        bRet = pngHuffmanTreeMakeFromLengths( pTree, punBitLen, NUM_DEFLATE_CODE_SYMBOLS, 15 );

        free( punBitLen );
    }

    return bRet;
}


void pngGetTreeInflateFixed( PngHuffmanTree *pTreeLLengths, PngHuffmanTree *pTreeDist )
{
    pngGenerateFixedBitLenTree( pTreeLLengths );
    pngGenerateFixedDistanceTree( pTreeDist );
}

unsigned char pngGetTreeInflateDynamic( PngHuffmanTree *pTreeLLengths, PngHuffmanTree *pTreeDist, const unsigned char* pbMem, size_t stMemSize, size_t *pstBitPtr )
{
    unsigned char bRet = 0;
    unsigned n, HLIT, HDIST, HCLEN, i;

    size_t stInBitLength = stMemSize * 8;

    //see comments in deflateDynamic for explanation of the context and these variables, it is analogous
    unsigned *punBitLenLL = 0; //lit,len code lengths
    unsigned *punBitLenDist = 0; //dist code lengths
    unsigned * punBitLenCL = 0; //code length code lengths ("clcl"), the bit lengths of the huffman tree used to compress punBitLenLL and punBitLenDist

   PngHuffmanTree TreeCLendth; //the code tree for code length codes (the huffman tree for compressed huffman trees)

    if ( ( *pstBitPtr ) + 14 > ( stMemSize << 3 ) )
        return 49; //error: the bit pointer is or will go past the memory

    //number of literal/length codes + 257. Unlike the spec, the value 257 is added to it here already
    HLIT = pngReadBitsFromStream( pstBitPtr, pbMem, 5 ) + 257;
    //number of distance codes. Unlike the spec, the value 1 is added to it here already
    HDIST = pngReadBitsFromStream( pstBitPtr, pbMem, 5 ) + 1;
    //number of code length codes. Unlike the spec, the value 4 is added to it here already
    HCLEN = pngReadBitsFromStream( pstBitPtr, pbMem, 4 ) + 4;

    if ( ( *pstBitPtr ) + HCLEN * 3 > ( stMemSize << 3 ) )
        return 50; //error: the bit pointer is or will go past the memory

    pngHuffmanTreeInit( &TreeCLendth );

    while ( !bRet )
    {
        //read the code length codes out of 3 * (amount of code length codes) bits

        punBitLenCL = (unsigned*)malloc( NUM_CODE_LENGTH_CODES * sizeof( unsigned ) );
        if ( !punBitLenCL )
        {
            bRet = 1;
            break;
        }

        for ( i = 0; i != NUM_CODE_LENGTH_CODES; ++i )
        {
            if ( i < HCLEN )
                punBitLenCL[ CLCL_ORDER[ i ] ] = pngReadBitsFromStream( pstBitPtr, pbMem, 3 );
            else
                punBitLenCL[ CLCL_ORDER[ i ] ] = 0; /*if not, it must stay 0*/
        }

        bRet = pngHuffmanTreeMakeFromLengths( &TreeCLendth, punBitLenCL, NUM_CODE_LENGTH_CODES, 7 );
        if ( bRet )
            break;

        /*now we can use this tree to read the lengths for the tree that this function will return*/
        punBitLenLL = (unsigned*)malloc( NUM_DEFLATE_CODE_SYMBOLS * sizeof( unsigned ) );
        punBitLenDist = (unsigned*)malloc( NUM_DISTANCE_SYMBOLS * sizeof( unsigned ) );

        if ( !punBitLenLL || !punBitLenDist )
        {
            bRet = 1;
            break;
        }

        for ( i = 0; i != NUM_DEFLATE_CODE_SYMBOLS; ++i )
            punBitLenLL[ i ] = 0;
        for ( i = 0; i != NUM_DISTANCE_SYMBOLS; ++i )
            punBitLenDist[ i ] = 0;

        //i is the current symbol we're reading in the part that contains the code lengths of lit/len and dist codes
        i = 0;
        while ( i < HLIT + HDIST )
        {
            unsigned code = pngHuffmanDecodeSymbol( pbMem, pstBitPtr, &TreeCLendth, stInBitLength );

            if ( code <= 15 ) //a length code
            {
                if ( i < HLIT )
                    punBitLenLL[ i ] = code;
                else
                    punBitLenDist[ i - HLIT ] = code;

                ++i;
            }
            else if ( code == 16 ) //repeat previous
            {
                unsigned replength = 3; //read in the 2 bits that indicate repeat length (3-6)
                unsigned value; //set value to the previous code

                if ( i == 0 )
                {
                    bRet = 54; //can't repeat previous if i is 0
                    break;
                }

                if ( ( *pstBitPtr + 2 ) > stInBitLength )
                {
                    bRet = 50; //error, bit pointer jumps past memory
                    break;
                }

                replength += pngReadBitsFromStream( pstBitPtr, pbMem, 2 );

                if ( i < HLIT + 1 ) value = punBitLenLL[ i - 1 ];
                else value = punBitLenDist[ i - HLIT - 1 ];
                //repeat this value in the next lengths
                for ( n = 0; n < replength; ++n )
                {
                    if ( i >= HLIT + HDIST )
                    {
                        bRet = 13; //error: i is larger than the amount of codes
                    }

                    if ( i < HLIT )
                        punBitLenLL[ i ] = value;
                    else
                        punBitLenDist[ i - HLIT ] = value;

                    ++i;
                }
            }
            else if ( code == 17 ) //repeat "0" 3-10 times
            {
                unsigned replength = 3; //read in the bits that indicate repeat length
                if ( ( *pstBitPtr + 3 ) > stInBitLength )
                {
                    bRet = 50; //error, bit pointer jumps past memory
                    break;
                }

                replength += pngReadBitsFromStream( pstBitPtr, pbMem, 3 );

                /*repeat this value in the next lengths*/
                for ( n = 0; n < replength; ++n )
                {
                    if ( i >= HLIT + HDIST )
                    {
                        bRet = 14; //error: i is larger than the amount of codes
                        break;
                    }

                    if ( i < HLIT )
                        punBitLenLL[ i ] = 0;
                    else
                        punBitLenDist[ i - HLIT ] = 0;

                    ++i;
                }
            }
            else if ( code == 18 ) //repeat "0" 11-138 times
            {
                unsigned replength = 11; //read in the bits that indicate repeat length
                if ( ( *pstBitPtr + 7 ) > stInBitLength )
                {
                    bRet = 50; //error, bit pointer jumps past memory
                    break;
                }

                replength += pngReadBitsFromStream( pstBitPtr, pbMem, 7 );

                //repeat this value in the next lengths
                for ( n = 0; n < replength; ++n )
                {
                    if ( i >= HLIT + HDIST )
                    {
                        bRet = 15; //error: i is larger than the amount of codes
                    }

                    if ( i < HLIT )
                        punBitLenLL[ i ] = 0;
                    else
                        punBitLenDist[ i - HLIT ] = 0;
                    ++i;
                }
            }
            else
            {
                if ( code == (unsigned)( -1 ) )
                {
                    //return error code 10 or 11 depending on the situation that happened in HuffmanDecodeSymbol
                    //(10=no endcode, 11=wrong jump outside of tree)
                    bRet = ( *pstBitPtr ) > stInBitLength ? 10 : 11;
                }
                else
                    bRet = 16; //unexisting code, this can never happen
                break;
            }
        }

        if ( bRet )
            break;

        if ( 0 == punBitLenLL[ 256 ] )
        {
            bRet = 64; //the length of the end code 256 must be larger than 0
            break;
        }

        //now we've finally got HLIT and HDIST, so generate the code trees, and the function is done
        if ( 0 == ( bRet = pngHuffmanTreeMakeFromLengths( pTreeLLengths, punBitLenLL, NUM_DEFLATE_CODE_SYMBOLS, 15 ) ) )
            bRet =pngHuffmanTreeMakeFromLengths( pTreeDist, punBitLenDist, NUM_DISTANCE_SYMBOLS, 15 );

        break; //end of error-while
    }

    free( punBitLenCL );
    free( punBitLenLL );
    free( punBitLenDist );

    pngHuffmanTreeCleanup( &TreeCLendth );

    return bRet;
}

unsigned char pngInflateHuffmanBlock( std::vector<unsigned char> &out, const unsigned char *pbMem, size_t stMemSize, size_t *pBPtr, size_t *pstPos, unsigned char bByteType )
{
    unsigned char                   bRet = 0;
    PngHuffmanTree                  TreeLLengths;        //literales y tamaño de codigos
    PngHuffmanTree                  TreeDist;             //codigos de distancias
    size_t                          stMemBitLength = stMemSize * 8;

    pngHuffmanTreeInit( &TreeLLengths );
    pngHuffmanTreeInit( &TreeDist );

    if ( 1 == bByteType )
       pngGetTreeInflateFixed( &TreeLLengths, &TreeDist );
    else if ( 2 == bByteType )
        bRet = pngGetTreeInflateDynamic( &TreeLLengths, &TreeDist, pbMem, stMemSize, pBPtr );

    while ( !bRet )
    {
        unsigned unCodeLL = pngHuffmanDecodeSymbol( pbMem, pBPtr, &TreeLLengths, stMemBitLength );
        if ( unCodeLL <= 255 )
        {
            out.resize((*pstPos) + 1);
            out[ *pstPos ] = (unsigned char)unCodeLL;
            ++( *pstPos );
        }
        else if ( unCodeLL >= FIRST_LENGTH_CODE_INDEX && unCodeLL <= LAST_LENGTH_CODE_INDEX )
        {
            unsigned unCodeDist, unDistance;
            unsigned unNumExtraBitsL, unNumExtraBitsD;

            size_t stStart, stForward, stBackward, stLength;

            stLength = LENGTHBASE[ unCodeLL - FIRST_LENGTH_CODE_INDEX ];

            unNumExtraBitsL = LENGTHEXTRA[ unCodeLL - FIRST_LENGTH_CODE_INDEX ];
            if ( ( *pBPtr + unNumExtraBitsL ) > stMemBitLength )
            {
                bRet = 51;
                break;
            }

            stLength += pngReadBitsFromStream( pBPtr, pbMem, unNumExtraBitsL );

            //Codigo de distancia
            unCodeDist = pngHuffmanDecodeSymbol( pbMem, pBPtr, &TreeDist, stMemBitLength );
            if ( unCodeDist > 29 )
            {
                if ( unCodeDist == (unsigned)( -1 ) )
                {
                    bRet = ( *pBPtr ) > stMemSize * 8 ? 10 : 11;
                }
                else
                    bRet = 18; //codigo de distancia erroneo

                break;
            }

            unDistance = DISTANCEBASE[ unCodeDist ];

            //Extra bits de distancias
            unNumExtraBitsD = DISTANCEEXTRA[ unCodeDist ];
            if ( ( *pBPtr + unNumExtraBitsD ) > stMemBitLength )
            {
                bRet = 51;
                break;
            }

            unDistance += pngReadBitsFromStream( pBPtr, pbMem, unNumExtraBitsD );

            stStart = ( *pstPos );
            if ( unDistance > stStart )
            {
                bRet = 52;
                break;
            }

            stBackward = stStart - unDistance;

            out.resize((*pstPos) + stLength);

            if ( unDistance < stLength )
            {
                for ( stForward = 0; stForward < stLength; ++stForward )
                    out[ ( *pstPos )++ ] = out[ stBackward++ ];
            }
            else
            {
                std::memcpy(out.data() + *pstPos, out.data() + stBackward, stLength);
                *pstPos += stLength;
            }
        }
        else if ( 256 == unCodeLL )
        {
            break;
        }
        else
        {
            //10 = sin endcode, 11 = salto fuera el arbol)
            bRet = ( ( *pBPtr ) > stMemSize * 8 ) ? 10 : 11;
            break;
        }
    }

    pngHuffmanTreeCleanup( &TreeLLengths );
    pngHuffmanTreeCleanup( &TreeDist );

    return bRet;
}

unsigned char pngInflateNoCompression(std::vector<unsigned char> &out, const unsigned char *pbMem, size_t *pBPtr, size_t *pstPos, size_t stMemSize )
{

    size_t stPos;
    unsigned LEN, NLEN, n;

    //ir al final del byte
    while ( ( ( *pBPtr ) & 0x7 ) != 0 )
        ++( *pBPtr );
    stPos = ( *pBPtr ) / 8; //byte pos

    // LEN (2 bytes) y NLEN (2 bytes)
    if ( stPos + 4 >= stMemSize )
        return 1;

    LEN = pbMem[ stPos ] + 256u * pbMem[ stPos + 1 ];
    stPos += 2;
    NLEN = pbMem[ stPos ] + 256u * pbMem[ stPos + 1 ];
    stPos += 2;

    //comprobar complemento
    if ( LEN + NLEN != 65535 )
        return 1;

    out.resize((*pstPos) + LEN);

    if ( stPos + LEN > stMemSize )
        return 1;

    //leer datos literales
    for ( n = 0; n < LEN; ++n )
        out[ ( *pstPos )++ ] = pbMem[ stPos++ ];

    ( *pBPtr ) = stPos * 8;

    return 0;
}


unsigned char pngInflate( unsigned char **psOut, size_t *pstOutSize, const unsigned char *pbMem, size_t stMemSize)
{
    unsigned char  bRet = 1;
    std::vector<unsigned char> vct;

    vct.reserve(*pstOutSize);
    for (size_t i = 0; i < *pstOutSize; i++)
        vct.push_back(*(*psOut + i));

    size_t bp = 0; //bit pointer
    unsigned BFINAL = 0;
    size_t stPos = 0; /*pos en el buffer de salida*/

    unsigned char BTYPE;
    unsigned char bBit;
    while ( !BFINAL )
    {

        if ( bp + 2 >= stMemSize * 8 )
            break;

        BFINAL = pngReadBitFromStream( &bp, pbMem );
        BTYPE = 1u * pngReadBitFromStream( &bp, pbMem );
        //BTYPE += (byte_t)(2u * ReadBitFromStream( &bp, pbMem ));

        bBit = pngReadBitFromStream( &bp, pbMem );
        BTYPE = (unsigned char)(BTYPE + (unsigned char)( bBit * 2u ));


        if ( BTYPE == 3 )
            break;
        else if ( BTYPE == 0 )
            bRet = pngInflateNoCompression( vct, pbMem, &bp, &stPos, stMemSize ); //sin compresion
        else
            bRet = pngInflateHuffmanBlock( vct, pbMem, stMemSize, &bp, &stPos, BTYPE ); //compresion BTYPE 01 o 10


        if ( bRet )
            break;

    }

    if ( 0 == bRet )
    {
        size_t i = 0;
        std::for_each(vct.begin(), vct.end(), [&](std::vector<unsigned char>::value_type& it) 
            { 
                (*psOut)[i++] = it; 
            }
        );
        
        *pstOutSize = vct.size();
    }

    return bRet;
}

/*
    Decompress
    zlib
*/

unsigned char pngDecompress( unsigned char **pbOut, size_t *pstOutSize, const unsigned char *pbMem, size_t stMemSize, PngDecoderSettings_t *pDecoder )
{
    unsigned char bRet = 1;
    unsigned char CM, CINFO, FDICT;

    if ( stMemSize > 2 )
    {
        if ( 0 == ( ( pbMem[ 0 ] * 256 + pbMem[ 1 ] ) % 31 ) )
        {
            CM = pbMem[ 0 ] & 15;
            CINFO = ( pbMem[ 0 ] >> 4 ) & 15;
            FDICT = ( pbMem[ 1 ] >> 5 ) & 1;
            if ( 8 == CM && CINFO <= 7 && 0 == FDICT )
            {
                bRet = pngInflate( pbOut, pstOutSize, pbMem + 2, stMemSize - 2/*, pDecoder*/ );
                if ( 0 == bRet && !pDecoder->bIgnoreAdler32 )
                {
                    unsigned long ADLER32 = draw::tools::to32bitData<unsigned long>( &pbMem[ stMemSize - 4 ] );
                    unsigned long checksum = draw::tools::crcI32( 0, *pbOut, (unsigned short)(*pstOutSize ));
                    bRet = ( checksum == ADLER32 ) ? 0 : 1;
                }
            }
        }
    }

    return bRet;
}


/*
*
            *****************************

*/



/*
          ************  CHUNKS ***************
*/

unsigned char pngChunkStringResize(char **psOut, size_t stSize )
{
    char *psData = (char*)realloc( *psOut, stSize + 1 );

    if ( psData )
    {
        psData[ stSize ] = 0;
        *psOut = psData;
    }

    return psData ? 1 : 0;
}

void pngChunkStringInit( char **psOut )
{
    *psOut = nullptr;
    pngChunkStringResize( psOut, 0 );
}

void pngChunkStringCleanup( char **psOut )
{
    free( *psOut );
    *psOut = nullptr;
}

void pngChunkStringSet( char **psOut, const char *szStr )
{
    size_t stStrSize = strlen( szStr );
    if ( pngChunkStringResize( psOut, stStrSize ) )
    {
        for ( size_t i = 0; i < stStrSize; i++ )
            ( *psOut )[ i ] = szStr[ i ];
    }
}



unsigned char pngAddChunkText( PngInfo_t* pInfo, const char *szKey, const char *szStr )
{
    unsigned char bRet = 1;
    char **psNewKeys = (char**)( realloc( pInfo->psTextKeys, sizeof( char* ) * ( pInfo->stTextNum + 1 ) ) );
    char **psNewStrings = (char**)( realloc( pInfo->psTextStrings, sizeof( char* ) * ( pInfo->stTextNum + 1 ) ) );

    if ( psNewKeys && psNewStrings )
    {

        pInfo->stTextNum++;

        pInfo->psTextKeys = psNewKeys;
        pInfo->psTextStrings = psNewStrings;

        pngChunkStringInit( &pInfo->psTextKeys[ pInfo->stTextNum - 1 ] );
        pngChunkStringSet( &pInfo->psTextKeys[ pInfo->stTextNum - 1 ], szKey );

        pngChunkStringInit( &pInfo->psTextStrings[ pInfo->stTextNum - 1 ] );
        pngChunkStringSet( &pInfo->psTextStrings[ pInfo->stTextNum - 1 ], szStr );

        bRet = 0;
    }

    return bRet;
}


unsigned char pngAddChunkiText( PngInfo_t* pInfo, const char *szKey, const char *szLangTag, const char *szTranskey, const char *szStr )
{

    char **sNewKeys = (char**)( realloc( pInfo->psiTextKeys, sizeof( char* ) * ( pInfo->stiTextNum + 1 ) ) );
    char **sNewLangTags = (char**)( realloc( pInfo->psiTextLangTags, sizeof( char* ) * ( pInfo->stiTextNum + 1 ) ) );
    char **sNewTranskeys = (char**)( realloc( pInfo->psiTextTranskeys, sizeof( char* ) * ( pInfo->stiTextNum + 1 ) ) );
    char **sNewStrings = (char**)( realloc( pInfo->psiTextStrings, sizeof( char* ) * ( pInfo->stiTextNum + 1 ) ) );

    if ( !sNewKeys || !sNewLangTags || !sNewTranskeys || !sNewStrings )
    {
        if ( sNewKeys )
            free( sNewKeys );
        if ( sNewLangTags )
            free( sNewLangTags );
        if ( sNewTranskeys )
            free( sNewTranskeys );
        if ( sNewStrings )
            free( sNewStrings );

        return 1;
    }

    ++pInfo->stiTextNum;
    pInfo->psiTextKeys = sNewKeys;
    pInfo->psiTextLangTags = sNewLangTags;
    pInfo->psiTextTranskeys = sNewTranskeys;
    pInfo->psiTextStrings = sNewStrings;

    pngChunkStringInit( &pInfo->psiTextKeys[ pInfo->stiTextNum - 1 ] );
    pngChunkStringSet( &pInfo->psiTextKeys[ pInfo->stiTextNum - 1 ], szKey );
    pngChunkStringInit( &pInfo->psiTextLangTags[ pInfo->stiTextNum - 1 ] );
    pngChunkStringSet( &pInfo->psiTextLangTags[ pInfo->stiTextNum - 1 ], szLangTag );
    pngChunkStringInit( &pInfo->psiTextTranskeys[ pInfo->stiTextNum - 1 ] );
    pngChunkStringSet( &pInfo->psiTextTranskeys[ pInfo->stiTextNum - 1 ], szTranskey );
    pngChunkStringInit( &pInfo->psiTextStrings[ pInfo->stiTextNum - 1 ] );
    pngChunkStringSet( &pInfo->psiTextStrings[ pInfo->stiTextNum - 1 ], szStr );

    return 0;
}


unsigned char pngChunk_PLTE( PngColorMode_t* pColor, const unsigned char *pbData, size_t stSize )
{

    if ( pColor->pbPalette )
        free( pColor->pbPalette );

    pColor->stPaletteSize = stSize / 3;
    pColor->pbPalette = (unsigned char*)malloc( 4 * pColor->stPaletteSize );

    if ( !pColor->pbPalette && pColor->stPaletteSize )
    {
        pColor->stPaletteSize = 0;
        return 1;
    }

    if ( pColor->stPaletteSize > 256 )
        return 1;

    size_t stPos = 0;
    for ( size_t i = 0; i < pColor->stPaletteSize; i++ )
    {
        pColor->pbPalette[ 4 * i + 0 ] = pbData[ stPos++ ];   //R
        pColor->pbPalette[ 4 * i + 1 ] = pbData[ stPos++ ];   //G
        pColor->pbPalette[ 4 * i + 2 ] = pbData[ stPos++ ];   //B
        pColor->pbPalette[ 4 * i + 3 ] = 255;               //A
    }

    return 0;
}

unsigned char pngChunk_tRNS( PngColorMode_t* pColor, const unsigned char* pbData, size_t stSize )
{

    if ( LCT_PALETTE == pColor->ColorType )
    {
        if ( stSize > pColor->stPaletteSize )
            return 1;

        for ( size_t i = 0; i < stSize; i++ )
            pColor->pbPalette[ 4 * i + 3 ] = pbData[ i ];
    }
    else if ( LCT_GREY == pColor->ColorType )
    {
        if ( 2 != stSize )
            return 1;

        pColor->bKeyDefined = 1;

        pColor->ulKeyR = (uint16_t)((uint16_t)(256u * pbData[ 0 ]) + pbData[ 1 ]);
        pColor->ulKeyG = pColor->ulKeyR;
        pColor->ulKeyB = pColor->ulKeyR;
    }
    else if ( LCT_RGB == pColor->ColorType )
    {
        if ( 6 != stSize )
            return 1;

        pColor->bKeyDefined = 1;
        pColor->ulKeyR = (uint16_t)( (uint16_t)(256u * ( pbData[ 0 ]) + pbData[ 1 ] ) );
        pColor->ulKeyG = (uint16_t)( (uint16_t)(256u * ( pbData[ 2 ]) + pbData[ 3 ] ) );
        pColor->ulKeyB = (uint16_t)( (uint16_t)(256u * ( pbData[ 4 ]) + pbData[ 5 ] ) );
    }
    else
        return 1;

    return 0;
}

unsigned char pngChunk_bKGD( PngInfo_t *pInfo, const unsigned char* pbData, size_t stSize )
{
    PngColorType ColorType = pInfo->Color.ColorType;

    if ( LCT_PALETTE == ColorType )
    {
        if ( 1 != stSize )
            return 1;

        pInfo->bBackgroundDefined = 1;
        pInfo->ulBackgroundB = pbData[ 0 ];
        pInfo->ulBackgroundR = pInfo->ulBackgroundB;
        pInfo->ulBackgroundG = pInfo->ulBackgroundB;
    }
    else if ( LCT_GREY == ColorType || LCT_GREY_ALPHA == ColorType )
    {
        if ( 2 != stSize )
            return 1;

        pInfo->bBackgroundDefined = 1;
        pInfo->ulBackgroundB = (uint16_t)((uint16_t)(256u * pbData[ 0 ]) + pbData[ 1 ]);
        pInfo->ulBackgroundR = pInfo->ulBackgroundB;
        pInfo->ulBackgroundG = pInfo->ulBackgroundB;
    }
    else if ( LCT_RGB == ColorType || LCT_RGBA == ColorType )
    {
        if ( 6 != stSize )
            return 1;

        pInfo->bBackgroundDefined = 1;
        pInfo->ulBackgroundR = (uint16_t)( ((uint16_t)256u * ( pbData[ 0 ] + pbData[ 1 ] ) ));
        pInfo->ulBackgroundG = (uint16_t)( ((uint16_t)256u * ( pbData[ 2 ] + pbData[ 3 ] ) ));
        pInfo->ulBackgroundB = (uint16_t)( ((uint16_t)256u * ( pbData[ 4 ] + pbData[ 5 ] ) ));
    }

    return 0;
}


unsigned char pngChunk_tEXt( PngInfo_t* pInfo, const unsigned char* pbData, size_t stDataSize )
{

    unsigned char  bError = 1;
    char  *szKey = nullptr;
    char  *szStr = nullptr;
    size_t  i;

    size_t  stLength;
    size_t  stStringBegin;

    stLength = 0;
    while ( stLength < stDataSize && pbData[ stLength ] != 0 )
        ++stLength;

    if ( stLength > 0 && stLength < 79 )
    {
        if (nullptr != ( szKey = (char*)malloc( stLength + 1 ) ) )
        {
            szKey[ stLength ] = 0;
            for ( i = 0; i != stLength; ++i )
                szKey[ i ] = (char)pbData[ i ];

            stStringBegin = stLength + 1;

            stLength = (unsigned)( stDataSize < stStringBegin ? 0 : stDataSize - stStringBegin );
            if (nullptr != ( szStr = (char*)malloc( stLength + 1 ) ) )
            {
                szStr[ stLength ] = 0;
                for ( i = 0; i != stLength; ++i )
                    szStr[ i ] = (char)pbData[ stStringBegin + i ];

                bError = pngAddChunkText( pInfo, szKey, szStr );

                if ( szKey )
                    free( szKey );

                if ( szStr )
                    free( szStr );
            }
        }
    }

    return bError;
}

unsigned char pngChunk_zTXt( PngState_t *pState, const unsigned char* pbData, size_t stDataSize )
{

    unsigned char bError = 1;

    size_t i;
    size_t stLength;
    size_t stStringBegin;

    char *szKey = nullptr;

    std::vector<unsigned char> vctDecoded;
  
    for ( stLength = 0; stLength < stDataSize && 0 != pbData[ stLength ]; ++stLength )
        ;

    if (
        ( stLength + 2 <= stDataSize ) &&
        stLength > 0 && stLength < 79
        )
    {
        if (nullptr != ( szKey = (char*)malloc( stLength + 1 ) ) )
        {
            szKey[ stLength ] = 0;
            for ( i = 0; i != stLength; ++i )
                szKey[ i ] = (char)pbData[ i ];

            if ( 0 == pbData[ stLength + 1 ] )
            {
                stStringBegin = stLength + 2;
                if ( stStringBegin < stDataSize )
                {
                    stLength = (unsigned)stDataSize - stStringBegin;

                    unsigned char* pbOut = &vctDecoded.front();
                    size_t stSize = vctDecoded.size();
                    if ( 0 == ( bError = pngDecompress( &pbOut, &stSize, (unsigned char*)(&pbData[stStringBegin]), stLength, &pState->Decoder)))
                    {
                        vctDecoded.push_back(0);
                        bError = pngAddChunkText( &pState->Info, szKey, (char*)vctDecoded.data());
                    }
                }
            }

            if ( szKey )
                free( szKey );
        }
    }

    vctDecoded.clear();
    return bError;
}


unsigned char pngReadChunk_iTXt( PngState_t *pState, const unsigned char *pbData, size_t stChunkLength )
{
    unsigned char bRet = 0;
    unsigned i;

    unsigned unLength, unBegin, unCompressed;

    char *sKey = nullptr;
    char *sLangTag = nullptr;
    char *sTranskey = nullptr;

    std::vector<unsigned char> vctDecoded;

    if ( stChunkLength > 5 )
    {
        for ( unLength = 0; unLength < stChunkLength && pbData[ unLength ] != 0; ++unLength )
            ;

        if ( unLength + 3 < stChunkLength )
        {

            if ( unLength && unLength < 79 )
            {

                sKey = (char*)malloc( unLength + 1 );
                if ( !sKey )
                    return 1;

                sKey[ unLength ] = 0;
                for ( i = 0; i != unLength; ++i )
                    sKey[ i ] = (char)pbData[ i ];

                /*read the compression method*/
                unCompressed = pbData[ unLength + 1 ];
                if ( 0 == pbData[ unLength + 2 ] )
                {

                    /*even though it's not allowed by the standard, no error is thrown if
                    there's no null termination char, if the text is empty for the next 3 texts*/

                    /*read the langtag*/
                    unBegin = unLength + 3;
                    unLength = 0;
                    for ( i = unBegin; i < stChunkLength && 0 != pbData[ i ]; ++i )
                        ++unLength;

                    sLangTag = (char*)malloc( unLength + 1 );
                    if ( !sLangTag )
                        return 1;

                    sLangTag[ unLength ] = 0;
                    for ( i = 0; i != unLength; ++i )
                        sLangTag[ i ] = (char)pbData[ unBegin + i ];

                    /*read the transkey*/
                    unBegin += unLength + 1;
                    unLength = 0;
                    for ( i = unBegin; i < stChunkLength && 0 != pbData[ i ]; ++i )
                        ++unLength;

                    sTranskey = (char*)malloc( unLength + 1 );
                    if ( !sTranskey )
                        return 1;

                    sTranskey[ unLength ] = 0;
                    for ( i = 0; i != unLength; ++i )
                        sTranskey[ i ] = (char)pbData[ unBegin + i ];

                    /*read the actual text*/
                    unBegin += unLength + 1;

                    unLength = (unsigned)stChunkLength < unBegin ? 0 : (unsigned)stChunkLength - unBegin;

                    if ( unCompressed )
                    {
                        unsigned char* pDcmpData = nullptr;
                        size_t stSize = 0;
                        if ( 0 == ( bRet = pngDecompress( &pDcmpData, &stSize, (unsigned char*)( &pbData[ unBegin ] ), unLength, &pState->Decoder ) ) )
                        {
                            vctDecoded.push_back(0);
                        }
                    }
                    else
                    {
                        vctDecoded.resize(unLength + 1);
                        vctDecoded[unLength] = 0;
                        for ( i = 0; i != unLength; ++i )
                            vctDecoded[i] = pbData[unBegin + i];
                    }

                    if ( 0 == bRet )
                        bRet = pngAddChunkiText( &pState->Info, sKey, sLangTag, sTranskey, (char*)vctDecoded.data());
                }
            }
        }

    }

    free( sKey );
    free( sLangTag );
    free( sTranskey );

    return bRet;
}


unsigned char pngChunk_Crc( const unsigned char *pbChunk )
{
    uint32_t udLength = draw::tools::to32bitData<unsigned long>( pbChunk );
    uint32_t udCRC = draw::tools::to32bitData<unsigned long>( &pbChunk[ udLength + 8 ] );
    uint32_t udSum = draw::tools::crcI32( 0, &pbChunk[ 4 ], (uint16_t)( udLength + 4 ) );

    return ( udCRC != udSum ) ? 1 : 0;
}


const unsigned char *pngChunk_NextConst( const unsigned char *pbChunk )
{
    uint32_t udChunkLength = draw::tools::to32bitData<unsigned long>( pbChunk ) + 12;
    return &pbChunk[ udChunkLength ];
}

size_t pngChunk_GetRawSizeiDat( size_t w, size_t h, const PngColorMode_t* pColor )
{
    size_t stBpp = pngGetBpp( pColor );
    size_t stLine = ( (size_t)( w / 8 ) * stBpp ) + 1 + ( ( w & 7 ) * stBpp + 7 ) / 8;
    return (size_t)h * stLine;
}

size_t pngChunk_GetRawSizeLct( unsigned w, unsigned h, PngColorMode_t* pColor )
{
    size_t stBpp = pngGetBpp( pColor );
    size_t n = (size_t)w * (size_t)h;
    return ( ( n / 8 ) * stBpp ) + ( ( n & 7 ) * stBpp + 7 ) / 8;
}


/*

    *****************************************************

*/





/*
    Safely check if multiplying two integers will overflow
    (no undefined behavior, compiler removing the code, etc...) and output result.
*/

static int mulofl( size_t a, size_t b, size_t* result )
{
    *result = a * b; /* Unsigned multiplication is well defined and safe in C90 */
    return ( a != 0 && *result / a != b );
}

/*
    Safely check if adding two integers will overflow (no undefined
    behavior, compiler removing the code, etc...) and output result.
*/
static int addofl( size_t a, size_t b, size_t* result )
{
    *result = a + b; /* Unsigned addition is well defined and safe in C90 */
    return *result < a;
}


void pngInit_ColorMode( PngColorMode_t *pColor )
{
    pColor->bKeyDefined = 0;
    pColor->ulKeyR = 0;
    pColor->ulKeyG = 0;
    pColor->ulKeyB = 0;
    pColor->ColorType = LCT_RGBA;
    pColor->bBitdepth = 8;
    pColor->pbPalette = nullptr;
    pColor->stPaletteSize = 0;
}

void pngState_Init( PngState_t *pState )
{
    pngInit_ColorMode( &pState->InfoRaw );
    pngInit_ColorMode( &pState->Info.Color );

    pState->Info.bInterlaceMethod = 0;
    pState->Info.bCompressionMethod = 0;
    pState->Info.bFilterMethod = 0;

    pState->Info.bBackgroundDefined = 0;
    pState->Info.ulBackgroundR = 0;
    pState->Info.ulBackgroundG = 0;
    pState->Info.ulBackgroundB = 0;

    pState->Info.stTextNum = 0;
    pState->Info.psTextKeys = nullptr;
    pState->Info.psTextStrings = nullptr;

    pState->Info.stiTextNum = 0;
    pState->Info.psiTextKeys = nullptr;
    pState->Info.psiTextLangTags = nullptr;
    pState->Info.psiTextTranskeys = nullptr;
    pState->Info.psiTextStrings = nullptr;

    pState->Decoder.bIgnoreAdler32 = 1;
    pState->Decoder.bReadTextChunks = 1;
    pState->Decoder.bColorConvert = 1;
    pState->Decoder.bIgnoreCrc = 0;
    pState->Decoder.bIgnoreCritical = 0;
    pState->Decoder.bIgnoreEnd = 0;
}


void pngCleanup_Palette( PngColorMode_t* pColorMode )
{
    if ( pColorMode->pbPalette )
        free( pColorMode->pbPalette );

    pColorMode->pbPalette = nullptr;
    pColorMode->stPaletteSize = 0;
}

void pngCleanup_iText( PngInfo_t* pInfo )
{
    size_t i;
    for ( i = 0; i < pInfo->stiTextNum; i++ )
    {
        free( pInfo->psiTextStrings[ i ] );
        free( pInfo->psiTextKeys[ i ] );
        free( pInfo->psiTextTranskeys[ i ] );
        free( pInfo->psiTextLangTags[ i ] );
    }

    free( pInfo->psiTextStrings );
    free( pInfo->psiTextKeys );
    free( pInfo->psiTextTranskeys );
    free( pInfo->psiTextLangTags );
}

void pngCleanup_Text( PngInfo_t* pInfo )
{
    size_t i;
    for ( i = 0; i < pInfo->stTextNum; i++ )
    {
        free( pInfo->psTextKeys[ i ] );
        free( pInfo->psTextStrings[ i ] );
    }

    free( pInfo->psTextKeys );
    free( pInfo->psTextStrings );
}

void pngCleanup_Info( PngInfo_t* pInfo )
{
    pngCleanup_Palette( &pInfo->Color );
    pngCleanup_iText( pInfo );
    pngCleanup_Text( pInfo );
}


void pngInfo_Init( PngInfo_t* pInfo )
{
    pngInit_ColorMode( &pInfo->Color );

    pInfo->bInterlaceMethod = 0;
    pInfo->bCompressionMethod = 0;
    pInfo->bFilterMethod = 0;
    pInfo->bBackgroundDefined = 0;
    pInfo->ulBackgroundR = 0;
    pInfo->ulBackgroundG = 0;
    pInfo->ulBackgroundB = 0;

    pInfo->stiTextNum = 0;
    pInfo->psiTextStrings = nullptr;
    pInfo->psiTextKeys = nullptr;
    pInfo->psiTextTranskeys = nullptr;
    pInfo->psiTextLangTags = nullptr;

    pInfo->stTextNum = 0;
    pInfo->psTextKeys = nullptr;
    pInfo->psTextStrings = nullptr;
}

unsigned char pngCheckColorValidity( PngColorType ColorType, unsigned char bBitdepth )
{
    unsigned char bRet = 0; //permitido

    switch ( ColorType )
    {
        case LCT_GREY:
            if ( 1 != bBitdepth && 2 != bBitdepth && 4 != bBitdepth && 8 != bBitdepth && 16 != bBitdepth )
                bRet = 1;
            break;
        case LCT_RGB:
            if ( 8 != bBitdepth && 16 != bBitdepth )
                bRet = 1;
            break;
        case LCT_PALETTE:
            if ( 1 != bBitdepth && 2 != bBitdepth && 4 != bBitdepth && 8 != bBitdepth )
                bRet = 1;
            break;
        case LCT_GREY_ALPHA:
            if ( 8 != bBitdepth && 16 != bBitdepth )
                bRet = 1;
            break;
        case LCT_RGBA:
            if ( 8 != bBitdepth && 16 != bBitdepth )
                bRet = 1;
            break;
        default:
            bRet = 1;
    }

    return bRet;
}

int pngColorModeCompare( const PngColorMode_t *a, const PngColorMode_t *b )
{
    size_t i;
    if ( a->ColorType != b->ColorType )
        return 0;
    if ( a->bBitdepth != b->bBitdepth )
        return 0;
    if ( a->bKeyDefined != b->bKeyDefined )
        return 0;

    if ( a->bKeyDefined )
    {
        if ( a->ulKeyR != b->ulKeyR )
            return 0;
        if ( a->ulKeyG != b->ulKeyG )
            return 0;
        if ( a->ulKeyB != b->ulKeyB )
            return 0;
    }

    if ( a->stPaletteSize != b->stPaletteSize )
        return 0;

    for ( i = 0; i != a->stPaletteSize * 4; ++i )
    {
        if ( a->pbPalette[ i ] != b->pbPalette[ i ] )
            return 0;
    }

    return 1;
}

unsigned char pngColorModeCopy( PngColorMode_t *pCModeDest, const PngColorMode_t *pCModeSource )
{
    size_t i;
    pngCleanup_Palette( pCModeDest );

    *pCModeDest = *pCModeSource;
    if ( pCModeSource->pbPalette )
    {
        pCModeDest->pbPalette = (unsigned char*)malloc( 1024 );

        if ( !pCModeDest->pbPalette && pCModeSource->stPaletteSize )
            return 1;

        for ( i = 0; i < pCModeSource->stPaletteSize * 4; ++i )
            pCModeDest->pbPalette[ i ] = pCModeSource->pbPalette[ i ];
    }

    return 0;
}


unsigned char pngInspect( unsigned long *w, unsigned long *h, PngState_t *pState, const unsigned char *pbyMem, size_t stMemSize )
{

    PngInfo_t *pInfo = &pState->Info;

    if ( stMemSize == 0 || NULL == pbyMem || stMemSize < 33 )
        return 1;

    pngCleanup_Info( pInfo );
    pngInfo_Init( pInfo );

    if ( pbyMem[ 0 ] != 137 || pbyMem[ 1 ] != 80 || pbyMem[ 2 ] != 78 || pbyMem[ 3 ] != 71
         || pbyMem[ 4 ] != 13 || pbyMem[ 5 ] != 10 || pbyMem[ 6 ] != 26 || pbyMem[ 7 ] != 10 )
    {
        return 1;
    }

    if ( 13 != draw::tools::to32bitData<unsigned long>( &pbyMem[ 8 ] ) )
        return 1;

    if ( !pngIsChunk( pbyMem + 8, "IHDR" ) )
        return 1;


    *w = (unsigned long)draw::tools::to32bitData<unsigned long>( &pbyMem[ 16 ] );
    *h = (unsigned long)draw::tools::to32bitData<unsigned long>( &pbyMem[ 20 ] );

    if ( *w == 0 || *h == 0 )
        return 1;

    pInfo->Color.bBitdepth = pbyMem[ 24 ];
    pInfo->Color.ColorType = (PngColorType)pbyMem[ 25 ];
    pInfo->bCompressionMethod = pbyMem[ 26 ];
    pInfo->bFilterMethod = pbyMem[ 27 ];
    pInfo->bInterlaceMethod = pbyMem[ 28 ];

    if ( 0 != pInfo->bFilterMethod || 0 != pInfo->bFilterMethod || pInfo->bInterlaceMethod > 1 )
        return 1;

    if ( !pState->Decoder.bIgnoreCrc )
    {
        if (draw::tools::to32bitData<unsigned long>( &pbyMem[ 29 ] ) != draw::tools::crcI32( 0, &pbyMem[ 12 ], 17 ) )
            return 1;
    }

    return pngCheckColorValidity( pInfo->Color.ColorType, pInfo->Color.bBitdepth );
}


/*
IsPixelOverflow:

    Comprueba de forma segura cantidad de píxeles puede haber overflow en un tipo de dato size_t
    Si esta funcion retorna 0 entonces se podra calcular de forma segura
        (size_t) w * (size_t) h * 8
        * cantidad de bytes en IDAT (incluyendo filtro, relleno y Adam7)
        * cantidad de bytes en el modelo de color sin procesar
*/

int pngIsPixelOverflow(
    unsigned w, unsigned h,
    const PngColorMode_t *pPngColor,
    const PngColorMode_t *pRawColor )
{

    size_t  stBpp = MAX_NUM( pngGetBpp( pPngColor ), pngGetBpp( pRawColor ) );
    size_t  stNumPixels;
    size_t  stTotal;
    size_t  stLine; //bytes por linea

    if ( mulofl( (size_t)w, (size_t)h, &stNumPixels ) )
        return 1;

    //puntero de bits con 8 bits por color u 8 byte por canal "channel color"
    if ( mulofl( stNumPixels, 8, &stTotal ) )
        return 1;

    // una linea (w / 8) * bpp) + ((w & 7) * bpp + 7) / 8 
    if ( mulofl( (size_t)( w / 8 ), stBpp, &stLine ) )
        return 1;

    if ( addofl( stLine, ( ( w & 7 ) * stBpp + 7 ) / 8, &stLine ) )
        return 1;

    //5 bytes overhead por linea: 1 filterbyte y 4 Adam7
    if ( addofl( stLine, 5, &stLine ) )
        return 1;

    //total bytes
    if ( mulofl( stLine, h, &stTotal ) )
        return 1;

    return 0;
}

unsigned char pngPaethPredictor( short a, short b, short c )
{
    short pa = (short)abs( b - c );
    short pb = (short)abs( a - c );
    short pc = (short)abs( a + b - c - c );

    if ( pc < pa && pc < pb )
        return (unsigned char)c;
    else if ( pb < pa )
        return (unsigned char)b;

    return (unsigned char)a;
}

/*
* UnfilterScanline
Para el método de filtro PNG 0
Anula el filtrado de una línea de escaneo de imagen PNG por línea de escaneo.
Cuando los píxeles son más pequeños que 1 byte, el filtro funciona byte por byte (stByteWidth = 1)
pbPrecon es la línea de escaneo anterior sin filtrar, reconocer el resultado, escanear la línea de escaneo actual.
las líneas de escaneo entrantes NO incluyen el byte de tipo de filtro, que se da en el parámetro bFilterType
*/

unsigned pngUnfilterScanline(
    unsigned char *pbRecon,
    const unsigned char *pbScanLine,
    const unsigned char *pbPrecon,
    size_t stByteWidth,
    unsigned char bFilterType,
    size_t stLength
)
{


    size_t i;
    switch ( bFilterType )
    {
        case 0:
            for ( i = 0; i < stLength; i++ )
                pbRecon[ i ] = pbScanLine[ i ];
            break;
        case 1:
            for ( i = 0; i < stByteWidth; i++ )
                pbRecon[ i ] = pbScanLine[ i ];

            for ( i = stByteWidth; i < stLength; i++ )
                pbRecon[ i ] = (unsigned char)(pbScanLine[ i ] + pbRecon[ i - stByteWidth ]);
            break;
        case 2:
            if ( pbPrecon )
            {
                for ( i = 0; i < stLength; i++ )
                    pbRecon[ i ] = (unsigned char)(pbScanLine[ i ] + pbPrecon[ i ]);
            }
            else
            {
                for ( i = 0; i < stLength; i++ )
                    pbRecon[ i ] = pbScanLine[ i ];
            }
            break;
        case 3:
            if ( pbPrecon )
            {
                for ( i = 0; i < stByteWidth; i++ )
                    pbRecon[ i ] = (unsigned char)(pbScanLine[ i ] + ( pbPrecon[ i ] >> 1 ));

                for ( i = stByteWidth; i < stLength; i++ )
                    pbRecon[ i ] = (unsigned char)(pbScanLine[ i ] + ( ( pbRecon[ i - stByteWidth ] + pbPrecon[ i ] ) >> 1 ));
            }
            else
            {
                for ( i = 0; i < stByteWidth; i++ )
                    pbRecon[ i ] = pbScanLine[ i ];
                for ( i = stByteWidth; i < stLength; i++ )
                    pbRecon[ i ] = (unsigned char)(pbScanLine[ i ] + ( pbRecon[ i - stByteWidth ] >> 1 ));
            }
            break;
        case 4:
            if ( pbPrecon )
            {
                for ( i = 0; i < stByteWidth; i++ )
                    pbRecon[ i ] = (unsigned char)( pbScanLine[ i ] + pbPrecon[ i ] );

                for ( i = stByteWidth; i < stLength; i++ )
                    pbRecon[ i ] = (unsigned char)( pbScanLine[ i ] + pngPaethPredictor( pbRecon[ i - stByteWidth ], pbPrecon[ i ], pbPrecon[ i - stByteWidth ] ) );
            }
            else
            {
                for ( i = 0; i < stByteWidth; i++ )
                    pbRecon[ i ] = pbScanLine[ i ];

                for ( i = stByteWidth; i < stLength; i++ )
                    pbRecon[ i ] = (unsigned char)( pbScanLine[ i ] + pbRecon[ i - stByteWidth ] );

            }
            break;
        default:
            return 1; //el filtro no existe
    }

    return 0;
}

/*
* Unfilter
Para el método de filtro PNG 0
esta función elimina el filtro de una sola imagen (por ejemplo, sin entrelazar, esto se llama una vez, con Adam7 siete veces)
pbOut debe tener suficientes bytes asignados, pbyMem debe tener las líneas de escaneo + 1 byte de tipo de filtro por línea de escaneo
w y h son dimensiones de imagen o dimensiones de imagen reducida, bpp son bits por píxel
Se permite que la entrada y la salida tengan la misma dirección de memoria (pero no son del mismo tamaño ya que tiene los bytes de filtro adicionales)
*/

unsigned pngUnfilter( unsigned char *pbOut, const unsigned char* pbyMem, size_t w, size_t h, size_t stBpp )
{
    unsigned char *pbPrevLine = nullptr;

    size_t stByteWidth = ( stBpp + 7 ) / 8;
    size_t stLineBytes = ( w * stBpp + 7 ) / 8;
    size_t stOutIndex;
    size_t stInIndex;

    unsigned char  bFilterType;

    for ( size_t y = 0; y < h; ++y )
    {
        stOutIndex = stLineBytes * y;
        stInIndex = ( 1 + stLineBytes ) * y;
        bFilterType = pbyMem[ stInIndex ];

        if ( pngUnfilterScanline( &pbOut[ stOutIndex ], &pbyMem[ stInIndex + 1 ], pbPrevLine, stByteWidth, bFilterType, stLineBytes ) )
            return 1;

        pbPrevLine = &pbOut[ stOutIndex ];
    }

    return 0;
}

void pngSetBitOfReversedStream( size_t *pstBitPtr, unsigned char* pbBitStream, unsigned char bit )
{
    if ( 0 == bit )
        pbBitStream[ ( *pstBitPtr ) >> 3 ] &= (unsigned char)( ~( 1 << ( 7 - ( ( *pstBitPtr ) & 0x7 ) ) ) );
    
        pbBitStream[ ( *pstBitPtr ) >> 3 ] |= (unsigned char)( 1 << ( 7 - ( ( *pstBitPtr ) & 0x7 ) ) );

    ++( *pstBitPtr );
}

/*
* RemovePaddingBits
*
*After filtering there are still padding bits if scanlines have non multiple of 8 bit amounts. They need
*to be removed (except at last scanline of (Adam7-reduced) image) before working with pure image buffers
*for the Adam7 code, the color convert code and the output to the user.
*in and out are allowed to be the same buffer, in may also be higher but still overlapping; in must
*have >= ilinebits*h bits, out must have >= olinebits*h bits, olinebits must be <= ilinebits
*also used to move bits after earlier such operations happened, e.g. in a sequence of reduced images from Adam7
*only useful if (ilinebits - olinebits) is a value in the range 1..7
*/
void pngRemovePaddingBits( unsigned char* pbOut, const unsigned char* pbyMem, size_t stOlineBits, size_t stIlineBits, unsigned h )
{

    unsigned y;
    size_t stDiff = stIlineBits - stOlineBits;
    size_t ibp = 0, obp = 0; //input / output bit pointers
    for ( y = 0; y < h; ++y )
    {
        size_t x;
        for ( x = 0; x < stOlineBits; ++x )
        {
            unsigned char bit = pngReadBitFromReversedStream( &ibp, pbyMem );
            pngSetBitOfReversedStream( &obp, pbOut, bit );
        }
        ibp += stDiff;
    }
}

/*
*
* PostProcessScanines
*This function converts the filtered-padded-interlaced data into pure 2D image buffer with the PNG's colortype.
*Steps:
**) if no Adam7: 1) unfilter 2) remove padding bits (= posible extra bits per scanline if bpp < 8)
**) if adam7: 1) 7x unfilter 2) 7x remove padding bits 3) Adam7_deinterlace
NOTE: the in buffer will be overwritten with intermediate data!
*/

unsigned char pngPostProcessScanines( unsigned char *pbOut, unsigned char *pbyMem, size_t w, size_t h, const PngInfo_t *pInfo )
{

    size_t stBpp = pngGetBpp( &pInfo->Color );

    if ( 0 == stBpp )
        return 31;

    if ( 0 == pInfo->bInterlaceMethod )
    {
        if ( stBpp < 8 && w * stBpp != ( ( w * stBpp + 7 ) / 8 ) * 8 )
        {
            if ( pngUnfilter( pbyMem, pbyMem, w, h, stBpp ) )
                return 1;

            pngRemovePaddingBits( pbOut, pbyMem, w * stBpp, ( ( w * stBpp + 7 ) / 8 ) * 8, h );
        }
        else
        {
            if ( pngUnfilter( pbOut, pbyMem, w, h, stBpp ) )
                return 1;
        }
    }
    else //entrelazado Adam7
    {
        unsigned passw[ 7 ], passh[ 7 ]; size_t filter_passstart[ 8 ], padded_passstart[ 8 ], passstart[ 8 ];
        unsigned i;

        pngAdam7GetPassValues( passw, passh, filter_passstart, padded_passstart, passstart, w, h, stBpp );

        for ( i = 0; i < 7; i++ )
        {
            if ( pngUnfilter( &pbyMem[ padded_passstart[ i ] ], &pbyMem[ filter_passstart[ i ] ], passw[ i ], passh[ i ], stBpp ) )
                return 1;

            if ( stBpp < 8 )
            {
                //Eliminar los bits de relleno en las lineas de exploracion, todavía puede haber bits de relleno entre las diferentes imágenes reducidas 
                pngRemovePaddingBits( &pbyMem[ passstart[ i ] ], &pbyMem[ padded_passstart[ i ] ], passw[ i ] * stBpp, ( ( passw[ i ] * stBpp + 7 ) / 8 ) * 8, passh[ i ] );
            }
        }

        pngAdam7Deinterlace( pbOut, pbyMem, w, h, stBpp );
    }

    return 0;
}


unsigned char pngDecode(
    unsigned char** ppbOut, unsigned long *w, unsigned long *h,
    PngState_t *pState,
    const unsigned char *pbyMem, size_t stMemSize
)
{

    unsigned char                   IEND = 0;
    const unsigned char            *pbChunk;
    uint32_t                        udChunkLength;
    const unsigned char            *pData;
    std::vector<unsigned char>      vctIdat;
    std::vector<unsigned char>      vctScanLines;
    size_t                          stPredict;
    size_t                          stOutSize = 0;
    size_t                          i;
    uint32_t                        stUnknown = 0;
    uint32_t                        stCriticalPos = 0;
    size_t                          stOldSize;
    size_t                          stNewSize;

    *ppbOut = 0;

   

    pState->bError = pngInspect( w, h, pState, pbyMem, stMemSize );
    if (
        0 == pState->bError &&
        0 == pngIsPixelOverflow( (uint32_t)*w, (uint32_t)*h, &pState->Info.Color, &pState->InfoRaw ) )
    {

        if(0 == stCriticalPos ) //esto es porque el compilador dice que no lo uso...
            stCriticalPos = 1; //1 = IHDR, 2 = PLTE, 3 = IDAT*/
        
        pbChunk = &pbyMem[ 33 ];

        /*
            Recorre los chunks ingnorando los "chunks desconocidos" hasta IEND
        */

        while ( !IEND && !pState->bError )
        {
            if ( (size_t)( ( pbChunk - pbyMem ) + 12 ) > stMemSize || pbChunk < pbyMem )
            {
                if ( pState->Decoder.bIgnoreEnd )
                    break;

                pState->bError = 1; //TODO: especificar si es necesario..
            }

            udChunkLength = draw::tools::to32bitData<unsigned long>( pbChunk );
            if ( ( udChunkLength = draw::tools::to32bitData<unsigned long>( pbChunk ) ) > 2147483647 )
            {
                if ( pState->Decoder.bIgnoreEnd )
                    break;

                pState->bError = 1; //TODO: especificar si es necesario..
            }

            if ( (size_t)(( (size_t)( pbChunk - pbyMem ) + (size_t)(udChunkLength + 12) )) > stMemSize || ( pbChunk + udChunkLength + 12 ) < pbyMem )
            {
                pState->bError = 1; //TODO: especificar si es necesario..
                break;
            }

            pData = &pbChunk[ 8 ];

            if ( pngIsChunk( pbChunk, "IDAT" ) )
            {
                //data comprimida de la imagen
                stOldSize = vctIdat.size();

                if ( addofl( stOldSize, udChunkLength, &stNewSize ) )
                {
                    pState->bError = 1; //TODO: especificar si es necesario..
                    break;
                }

                vctIdat.resize(stNewSize);

                for ( i = 0; i != udChunkLength; ++i )
                    vctIdat[ stOldSize + i ] = pData[ i ];

                stCriticalPos = 3;
            }
            else if ( pngIsChunk( pbChunk, "IEND" ) )
            {
                IEND = 1;
            }
            else if ( pngIsChunk( pbChunk, "PLTE" ) )
            {
                if ( ( pState->bError = pngChunk_PLTE( &pState->Info.Color, pData, udChunkLength ) ) > 0 )
                    break;

                stCriticalPos = 2;
            }
            else if ( pngIsChunk( pbChunk, "tRNS" ) )
            {
                if ( ( pState->bError = pngChunk_tRNS( &pState->Info.Color, pData, udChunkLength ) ) > 0 )
                    break;
            }
            else if ( pngIsChunk( pbChunk, "bKGD" ) )
            {
                if ( ( pState->bError = pngChunk_bKGD( &pState->Info, pData, udChunkLength ) ) > 0 )
                    break;
            }
            else if ( pngIsChunk( pbChunk, "tEXt" ) )
            {
                if ( pState->Decoder.bReadTextChunks )
                {
                    if ( ( pState->bError = pngChunk_tEXt( &pState->Info, pData, udChunkLength ) ) > 0 )
                        break;
                }
            }
            else if ( pngIsChunk( pbChunk, "zTXt" ) )
            {

                if ( pState->Decoder.bReadTextChunks )
                {
                    if ( ( pState->bError = pngChunk_zTXt( pState, pData, udChunkLength ) ) > 0 )
                        break;
                }

            }
            else if ( pngIsChunk( pbChunk, "iTXt" ) )
            {
                if ( pState->Decoder.bReadTextChunks )
                {
                    if ( ( pState->bError = pngReadChunk_iTXt( pState, pData, udChunkLength ) ) > 0 )
                        break;
                }
            }
            else
            {
                if ( !pState->Decoder.bIgnoreCritical && !( ( pbChunk[ 4 ] & 32 ) != 0 ) )
                {
                    pState->bError = 1; //TODO: especificar si es necesario..
                    break;
                }

                stUnknown = 1;
            }

            if ( !pState->Decoder.bIgnoreCrc && !stUnknown )
            {
                if ( pngChunk_Crc( pbChunk ) )
                {
                    pState->bError = 1; //TODO: especificar si es necesario..
                    break;
                }
            }

            if ( !IEND )
                pbChunk = pngChunk_NextConst( pbChunk );
        }

        if ( 0 == pState->Info.bInterlaceMethod )
        {
            stPredict = pngChunk_GetRawSizeiDat( (size_t)*w, (size_t)*h, &pState->Info.Color );
        }
        else
        {
            //FUKING Adam-7!!!
            const PngColorMode_t *pColor = &pState->Info.Color;
            stPredict = 0;
            stPredict += pngChunk_GetRawSizeiDat( (size_t)(( *w + 7 ) >> 3), (size_t)(( *h + 7 ) >> 3), pColor );
            if ( *w > 4 )
                stPredict += pngChunk_GetRawSizeiDat( (size_t)(( *w + 3 ) >> 3), (size_t)(( *h + 7 ) >> 3), pColor );

            stPredict += pngChunk_GetRawSizeiDat( (size_t)(( *w + 3 ) >> 2), (size_t)(( *h + 3 ) >> 3), pColor );
            if ( *w > 2 )
                stPredict += pngChunk_GetRawSizeiDat( (size_t)(( *w + 1 ) >> 2), (size_t)(( *h + 3 ) >> 2), pColor );

            stPredict += pngChunk_GetRawSizeiDat( (size_t)(( *w + 1 ) >> 1), (size_t)(( *h + 1 ) >> 2), pColor );
            if ( *w > 1 )
                stPredict += pngChunk_GetRawSizeiDat( (size_t)(( *w + 0 ) >> 1), (size_t)(( *h + 1 ) >> 1), pColor );

            stPredict += pngChunk_GetRawSizeiDat( (size_t)( *w ), (size_t)(( *h ) >> 1), pColor );
        }

        vctScanLines.resize(stPredict);

        if ( !pState->bError )
        {
//            unsigned char* pOut = vctScanLines.data();
            size_t size = vctScanLines.size();
            unsigned char* pOut = new unsigned char[size];
            pState->bError = pngDecompress( &pOut, &size, vctIdat.data(), vctIdat.size(), &pState->Decoder );

            for (size_t i = 0; i < size; i++)
                vctScanLines[i] = *(pOut + i);

            delete[] pOut;

            if ( !pState->bError && vctScanLines.size() != stPredict )
                pState->bError = 1; //el tamaño de la descompresion no conincide con el predictivo...
        }

        vctIdat.clear();

        if ( !pState->bError )
        {
            stOutSize = pngChunk_GetRawSizeLct( (uint32_t)*w, (uint32_t)*h, &pState->Info.Color );
            *ppbOut = (unsigned char*)malloc( stOutSize );
            if ( !*ppbOut )
                pState->bError = 1;
        }
        if ( !pState->bError )
        {
            for ( i = 0; i < stOutSize; i++ )
                ( *ppbOut )[ i ] = 0;

            pState->bError = pngPostProcessScanines( *ppbOut, vctScanLines.data(), (size_t)*w, (size_t)*h, &pState->Info );
        }

        vctScanLines.clear();

        if ( 0 == pState->bError )
        {
            unsigned char bColorConvert = pState->Decoder.bColorConvert;

            if ( !bColorConvert || pngColorModeCompare( &pState->InfoRaw, &pState->Info.Color ) )
            {
                //	mismo tipo color, no es necesario convertir la data
                if ( !bColorConvert )
                    pState->bError = pngColorModeCopy( &pState->InfoRaw, &pState->Info.Color );
            }
            else
            {
                //TODO: hay que convertir
            }
        }
    }

    return pState->bError;
}

/*
 * Decode_File
 */
bool  pngDecode_File(
    unsigned char                  **ppbOut,
    unsigned long                  *w,
    unsigned long                  *h,
    const char                     *szPath,
    PngColorType                    ColorType,
    unsigned char                   bBitdepth

)
{
    bool          bRet = false;

    std::ifstream file(szPath, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        size_t size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer;
        buffer.reserve(size);

        file.seekg(0, std::ios::beg);

        if (file.read(buffer.data(), size))
        {
            PngState_t State;
            pngState_Init(&State);
            State.InfoRaw.ColorType = ColorType;
            State.InfoRaw.bBitdepth = bBitdepth;

            bRet = (0 == pngDecode(ppbOut, w, h, &State, (const unsigned char*)(buffer.data()), size)); 

            pngCleanup_Palette(&State.InfoRaw);
            pngCleanup_Info(&State.Info);
        }

        file.close();

    }


    return bRet;
}


Png::~Png()
{
    if (pRawData)
    {
        free(pRawData);
        pRawData = nullptr;
    }
}

bool Png::load(const char* szFile, unsigned char bpp, unsigned char byBitdepth)
{
    unsigned char* pbImgData{nullptr};
    
    if (pngDecode_File(&pbImgData, &ulWidth, &ulHeight, szFile, LCT_RGBA, byBitdepth)) //asumo que todas tienen canal alpha
    {
        byBpp = bpp;
        pRawData = pbImgData;
        ulSize = (ulWidth * ulHeight * (byBpp >> 3));
        return true;
    }

    return false;
}


