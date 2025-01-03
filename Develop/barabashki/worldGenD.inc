        ;������ ��� ����� �����
        _buffer db 257 dup(0)
        ;���������� - ������������ ����� � ������� ����
        _gvPackageName db "C:\BlinEra-repos\BlinEra",40 dup 0
        _testF db 'C:\testF.txt',0
        _tempB db 0
        _nextStr db 13,10
        ;�������� ����� �����
        _gvBConfigName db 'BiomeConfig.json',0
        _gvSlashStr    db '\',0
        _gvWorldsDirName db 'Worlds',0
        ;������� ��� ���� ��� ���������
        _gvSeed   dd 444,0
        _gvWorldName db "�7Z�",16 dup 0
        ; �������� ���� ����� ������
        meadow biomeData        nil, 'meadow',DIRT,DIVOT
        taiga  biomeData        nil, 'taiga',DIRT,DIVOT
        desert biomeData        nil, 'desert',SAND,SAND
        swamp  biomeData        nil, 'swamp',DIRT,DIVOT
        dwarfMounts  biomeData  nil, 'dwarfMounts',STONE,STONE
        goblinMounts biomeData  nil, 'goblinMounts',STONE,STONE
        beauty biomeData        nil, 'beauty',DIRT,DIVOT
        jungle biomeData        nil, 'jungle',DIRT,DIVOT
        
        ;������ ���� ����� ������
        biomeTable dd meadow,taiga,desert,swamp,dwarfMounts,goblinMounts,beauty,jungle
        
        ;��������� ���������� - �� � ����� 
        temp    dd ?
        
        ;��������� �������������� �����
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
        
        ;������ ��������� ��������������� �����
        txtTable dd txtMax,txtMin,txtDifFrequecy,txtMaxDifference,txtSize,txtTreeType,\
                    txtTreePeriod,txtPuddlePeriod,txtPuddleSize,txtHolePeriod
           
        ;������ �������� � ������            
        offsTable db max,min,difFrequecy,maxDifference,size,treeType,treePeriod,\
                     puddlePeriod,puddleSize,holePeriod
        
        ;����� ������������ ����� ������� 
        biomesEndAddr dd ?
        ;���������� ������
        biomeCount dd 10
        ;������ ������, �������� � ����
        biomes db biomeInfoSize*maxBiomeCount dup(0)
        
        hillsCount dd 20
        hills db landscInfoSize*maxHillCount dup(0)
        
        woodsCount dd 30
        woods db landscInfoSize*maxWoodCount dup(0)
        
        ;������ ����
        worldWidth dd 3000
        
        worldPoints dd ?
        
        worldSplain splInfo
        
        _genNaD_Table dd 0,0,0,1,0,0,1,1,1,\
                         0,0,0,1,1,0,1,1,1
        
        PerlinDistTo1 db 63,63
        PerlinDistTo2 db -63,63
        PerlinDistTo3 db 63,-63
        PerlinDistTo4 db -63,-63        
