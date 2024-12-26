        _imPackageName db 80 dup 0
        _imTexturesPack db "Textures",0
        typesColor db grayScale, trueColor, indexedColor, grayScaleA, trueColorA
        IDAT db 'IDAT'
        windowSize dw 256, 512, 1024, 2048, 4096, 8192, 16384, 32768     
        _signature db 89h, 50h, 4Eh, 47h, 0Dh, 0Ah, 1Ah, 0Ah 
        _imSlash db "\",0
        _fullFName db 1024 dup ?
        
        from0      PackedValueInfo 0, 8, 00110000b, 10111111b
        from144    PackedValueInfo 144, 9, 110010000b, 111111111b
        from256    PackedValueInfo 256, 7, 0000000b, 0010111b
        from280    PackedValueInfo 280, 8, 11000000b, 11000111b   
        
        
        fixedHTable dd from256, from0, from280, from144
        
        zero        OffsetLZ77 0,1
        one         OffsetLZ77 0,2              
        two         OffsetLZ77 0,3
        three       OffsetLZ77 0,4
        four        OffsetLZ77 1,5
        five        OffsetLZ77 1,7
        six         OffsetLZ77 2,9
        seven       OffsetLZ77 2,13
        eight       OffsetLZ77 3,17
        nine        OffsetLZ77 3,25
        ten         OffsetLZ77 4,33
        elev        OffsetLZ77 4,49
        twelve      OffsetLZ77 5,65
        thirty      OffsetLZ77 5,97
        fourty      OffsetLZ77 6,129
        fifty       OffsetLZ77 6,193
        sixty       OffsetLZ77 7,257
        seventy     OffsetLZ77 7,385
        eightee     OffsetLZ77 8,513
        ninety      OffsetLZ77 8,769
        twenty      OffsetLZ77 9,1025
        twOne       OffsetLZ77 9,1537
        twTwo       OffsetLZ77 10,2049
        twThree     OffsetLZ77 10,3073
        twFour      OffsetLZ77 11,4097
        twFive      OffsetLZ77 11,6145
        twSix       OffsetLZ77 12,8193
        twSeven     OffsetLZ77 12,12289
        twEight     OffsetLZ77 13,16385
        twNine      OffsetLZ77 13,24577
        
        reverseOffsTable dd zero,one, two, three, four, five, six, seven, eight, nine,\
                            ten, elev, twelve, thirty, fourty, fifty, sixty, seventy,\
                            eightee, ninety, twenty, twOne, twTwo, twThree, twFour,\
                            twFive, twSix, twSeven, twEight, twNine
                            
        zero_        LengthLZ77 0,3
        one_         LengthLZ77 0,4              
        two_         LengthLZ77 0,5
        three_       LengthLZ77 0,6
        four_        LengthLZ77 0,7
        five_        LengthLZ77 0,8
        six_         LengthLZ77 0,9
        seven_       LengthLZ77 0,10
        eight_       LengthLZ77 1,11
        nine_        LengthLZ77 1,13
        ten_         LengthLZ77 1,15
        elev_        LengthLZ77 1,17
        twelve_      LengthLZ77 2,19
        thirty_      LengthLZ77 2,23
        fourty_      LengthLZ77 2,27
        fifty_       LengthLZ77 2,31
        sixty_       OffsetLZ77 3,35
        seventy_     LengthLZ77 3,43
        eightee_     LengthLZ77 3,51
        ninety_      LengthLZ77 3,59
        twenty_      LengthLZ77 4,67
        twOne_       LengthLZ77 4,83
        twTwo_       LengthLZ77 4,99
        twThree_     LengthLZ77 4,115
        twFour_      LengthLZ77 5,131
        twFive_      LengthLZ77 5,163
        twSix_       LengthLZ77 5,195
        twSeven_     LengthLZ77 5,227
        twEight_     LengthLZ77 0,258
        
        lengthTable dd      zero_, one_, two_, three_, four_, five_, six_, seven_, eight_, nine_,\
                            ten_, elev_, twelve_, thirty_, fourty_, fifty_, sixty_, seventy_,\
                            eightee_, ninety_, twenty_, twOne_, twTwo_, twThree_, twFour_,\
                            twFive_, twSix_, twSeven_, twEight_
                            
                            
        sixteenSymb CLen 16
        seventeenSymb CLen 17
        eighteenSymb CLen 18
        zeroSymb CLen 0
        eightSymb CLen 8
        sevenSymb CLen 7
        nineSymb CLen 9
        sixSymb CLen 6
        tenSymb CLen 10
        fiveSymb CLen 5
        elevenSymb CLen 11
        fourSymb CLen 4
        twelveSymb CLen 12
        threeSymb CLen 3
        thirtySymb CLen 13
        twoSymb CLen 2
        fourtySymb CLen 14
        oneSymb CLen 1
        fiftySymb CLen 15
        
        bufSixteenSymb CLen 16,-1,-1
        bufSeventeenSymb CLen 17,-1,-1
        bufEighteenSymb CLen 18,-1,-1
        bufZeroSymb CLen 0,-1,-1
        bufEightSymb CLen 8,-1,-1
        bufSevenSymb CLen 7,-1,-1
        bufNineSymb CLen 9,-1,-1
        bufSixSymb CLen 6,-1,-1
        bufTenSymb CLen 10,-1,-1
        bufFiveSymb CLen 5,-1,-1
        bufElevenSymb CLen 11,-1,-1
        bufFourSymb CLen 4,-1,-1
        bufTwelveSymb CLen 12,-1,-1
        bufThreeSymb CLen 3,-1,-1
        bufThirtySymb CLen 13,-1,-1
        bufTwoSymb CLen 2,-1,-1
        bufFourtySymb CLen 14,-1,-1
        bufOneSymb CLen 1,-1,-1
        bufFiftySymb CLen 15,-1,-1
             
        CLenTable dd sixteenSymb,seventeenSymb,eighteenSymb,zeroSymb,eightSymb,\
            sevenSymb,nineSymb,sixSymb,tenSymb,fiveSymb,elevenSymb,fourSymb,\
            twelveSymb,threeSymb,thirtySymb,twoSymb,fourtySymb,oneSymb,fiftySymb
            
        BufCLenTable dd bufSixteenSymb,bufSeventeenSymb,bufEighteenSymb,bufZeroSymb,bufEightSymb,\
            bufSevenSymb,bufNineSymb,bufSixSymb,bufTenSymb,bufFiveSymb,bufElevenSymb,bufFourSymb,\
            bufTwelveSymb,bufThreeSymb,bufThirtySymb,bufTwoSymb,bufFourtySymb,bufOneSymb,bufFiftySymb  
        
           
        width dd 8
        height dd 8

        colors dd blockWidth*blockHeight dup ?
        
        fHandle dd ?
        strScan db ?
        slideWin dw ?