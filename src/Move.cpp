#include"Move.h"
#include<string>
// constructor 
Move::Move(){
    fromRow = 0;
    fromCol = 0;
    toRow=0;
    toCol=0;
    piece=EMPTY;
    captured=EMPTY;
    promotion=EMPTY;
    isEnPassant=false;
    isCastle=false;
}
Move::Move(int fr,int fc,int tr,int tc,char p,char cap,char promo,bool ep,bool castle){
    fromRow=fr;
    fromCol=fc;
    toRow=tr;
    toCol=tc;
    piece=p;
    captured=cap;
    promotion=promo;
    isEnPassant=ep;
    isCastle=castle;
}
std::string Move::toAlgebraic() const{
    //convert the current move to uci format
    std::string s;
    s+=static_cast<char>('a'+fromCol);
    s+=static_cast<char>('8'-fromRow);
    s+=static_cast<char>('a'+toCol);
    s+=static_cast<char>('8'-toRow);
    if(promotion!=EMPTY&&(std::tolower(piece) =='p') &&(toRow==0||toRow==7)){
        // the promotion piece should be lower case 
        s+=static_cast<char>(std::tolower(promotion));
    }
    return s;
}
bool Move::operator==(const Move& other) const{
    return fromRow==other.fromRow && fromCol==other.fromCol && toRow==other.toRow && toCol==other.toCol && other.promotion==promotion;
}
