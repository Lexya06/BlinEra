        ;буффер для любых задач
        _buffer db 257 dup(0)
        ;перепенная - расположение папки с файлами игры
        _gvPackageName db "C:\BlinEra-repos\BlinEra",40 dup 0
        _testF db 'C:\testF.txt',0
        _tempB db 0
        _nextStr db 13,10
        ;название кофиг файла
        _gvBConfigName db 'BiomeConfig.json',0
        _gvSlashStr    db '\',0
        _gvWorldsDirName db 'Worlds',0
        ;текущий сид мира для генерации
        _gvSeed   dd 444,0
        _gvWorldName db "з7Zє",16 dup 0
        ; описание всех типов биомов
        meadow biomeData        nil, 'meadow',DIRT,DIVOT
        taiga  biomeData        nil, 'taiga',DIRT,DIVOT
        desert biomeData        nil, 'desert',SAND,SAND
        swamp  biomeData        nil, 'swamp',DIRT,DIVOT
        dwarfMounts  biomeData  nil, 'dwarfMounts',STONE,STONE
        goblinMounts biomeData  nil, 'goblinMounts',STONE,STONE
        beauty biomeData        nil, 'beauty',DIRT,DIVOT
        jungle biomeData        nil, 'jungle',DIRT,DIVOT
        
        ;массив всех типов биомов
        biomeTable dd meadow,taiga,desert,swamp,dwarfMounts,goblinMounts,beauty,jungle
        
        ;буфферная переменная - ну а вдруг 
        temp    dd ?
        
        ;строковые идентификаторы полей
        txtMax db "max",0
        txtMin db "min",0
        txtDifFrequecy db "difFrequecy",0
        txtMaxDifference db "maxDifference",0
        txtSize db "size",0
        txtTreeType db "treeType",0
        txtTreePeriod db "treePeriod",0
        txtPuddlePeriod db "puddlePeriod",0
        txtPuddleSize db "puddleSize",0
        txtHolePeriod db "holePeriod",0
        
        ;массив строковых идентификаторов полей
        txtTable dd txtMax,txtMin,txtDifFrequecy,txtMaxDifference,txtSize,txtTreeType,\
                    txtTreePeriod,txtPuddlePeriod,txtPuddleSize,txtHolePeriod
           
        ;массив смещений в записи            
        offsTable db max,min,difFrequecy,maxDifference,size,treeType,treePeriod,\
                     puddlePeriod,puddleSize,holePeriod
        
        ;адрес фактического конца массива 
        biomesEndAddr dd ?
        ;количество биомов
        biomeCount dd 10
        ;массив биомов, будующих в мире
        biomes db biomeInfoSize*maxBiomeCount dup(0)
        
        hillsCount dd 20
        hills db landscInfoSize*maxHillCount dup(0)
        
        woodsCount dd 30
        woods db landscInfoSize*maxWoodCount dup(0)
        
        ;ширина мира
        worldWidth dd 3000
        
        worldPoints dd ?
        
        worldSplain splInfo
        
        _genNaD_Table dd 0,0,0,1,0,0,1,1,1,\
                         0,0,0,1,1,0,1,1,1
        
        PerlinDistTo1 db 63,63
        PerlinDistTo2 db -63,63
        PerlinDistTo3 db 63,-63
        PerlinDistTo4 db -63,-63        
