public int GetPrintTwins(int number){
    int count=0;
    int lastNum=0;//表示上一个素数，默认0
    for(var i=2;i<=number;i++){
        //默认所有的数字都是素数
        bool isSS=true;
        for(var j=2;j<=i/2;j++){
            if(i%j==0){
                isSS=false;
                break;
            }
        }
        //i是一个素数
        if(isSS){
            //上一素数存在
            if(lastNum!=0){
                if(i-lastNum==2)
                {
                    count++;
                }
            }
            lastNum=i;
        }

    }
    return count;
}
