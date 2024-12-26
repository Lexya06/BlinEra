        ;количество типов биомов
        biomeTypesCount = 8
        ;количество параметров для генерации биомов
        biomeGenParCount = 10
        ;максимальное количество биомов
        maxBiomeCount = 32
        
        maxHillCount = maxBiomeCount*2
        maxWoodCount = maxBiomeCount*3
        
        maxHoleWidth = 12
        minHoleWidth = 4
        
        nil = 0
        
        TR_OAK = 0
        TR_PINE = 1
        TR_CACTUS = 2
        TR_TROPIC = 4
        
        ;смещения параметров в записи
        max = 0
        min = 2
        difFrequecy = 4
        maxDifference = 6
        size = 8
        treeType = 10
        treePeriod = 12
        puddlePeriod = 14
        puddleSize = 16
        holePeriod = 18

        ;макрос для получения случайного числа
        macro randomINLINE seed
        {
               mov     eax,[seed]
               mov     ecx,[seed]
               add     ecx, eax
               shl     ecx,1
               adc     eax,ecx
               rcr     eax,13
               mov     [seed],eax
        }

        macro randomInlineLITE regRes
        {
               mov     ecx,regRes
               shl     ecx,1
               adc     regRes,ecx
               rcr     regRes,13 
        } 
        ;обмен двух 4б переменных
        macro swapD arg1,arg2
        {
               mov     eax,[arg1]
               xchg    [arg2],eax
               mov     [arg1],eax 
        }
        
        macro sumAround regPos,regStrSize
        {  
               push   ebx
               lea    ebx,regPos
               mov    eax,[ebx-4]
               add    eax,[ebx+4]
               shl    regStrSize,2
               sub    ebx,regStrSize
               add    eax,[ebx]
               add    eax,[ebx-4]
               add    eax,[ebx+4]
               add    ebx,regStrSize 
               add    ebx,regStrSize
               add    eax,[ebx]
               add    eax,[ebx-4]
               add    eax,[ebx+4]
               shr    regStrSize,2
               pop    ebx
        }
        
        ;смещение названия биома в структуре
        biomeNameOffs = 4
        biomeBottomBlockOffs = 20
        biomeUpperBlockOffs = 24
        ;максимальный размер названия биома
        biomeNameSize = 15
        
        ;размер структуры с данными ТИПА биома
        biomeDataSize = 28
        ;общая характеристика боиомов этого ТИПА
        struct biomeData
               genInfo  dd nil            
               biomeName db 16 dup(0)
               bottomBlock dd STONE
               upperBlock dd STONE
        ends
        
        ;описания структуры биома
        biomeInfoSize = 12
        relativeSize = 4
        startPoint = 6
        endPoint = 10
        struct biomeInfo
               typeData dd nil
               relativeSize dw 0
               startPoint dw 0
               size dw 0
               endPoint dw 0
        ends
        
        ;описание структуры ландшафта
        landscInfoSize = 4
        landscStartPoint = 0
        landscEndPoint = 2
        struct landscInfo
               startPoint dw 0
               endPoint dw 0
        ends
        
        worldHeight = 1000
        worldZeroHeight = 400
        
        xOffs = 4
        yOffs = 8
        hOffs = 12
        bOffs = 16
        cOffs = 20
        dOffs = 24
        struct splInfo
               colPoints dd 0
               x dd ?
               y dd ?
               h dd ?
               b dd ?
               c dd ?
               d dd ?
        ends