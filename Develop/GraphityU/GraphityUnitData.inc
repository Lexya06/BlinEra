main dd ?

blockNamesTable dd airN,stoneN,dirtyN,divotN,sandDynamicN,oakWoodN,oakFoliageN,pineWoodN,pineFoliageN,tropicWoodN,tropicFoliageN,cactusN,cactusUpN
blockTexturesTable dd airT,stoneT,dirtyT,divotT,sandDynamicT,oakWoodT,oakFoliageT,pineWoodT,pineFoliageT,tropicWoodT,tropicFoliageT,cactusT,cactusUpT



wallsNamesTable dd airN,stoneN,dirtyN,divotN,sandDynamicN 
wallsTexturesTable dd airT,stoneT,dirtyT,divotT,sandDynamicT



backGroundN db 'Background.png',0
backGroundNamesTable dd backGroundN
backgroundT dd blockWidth*blockHeight dup ?
backgroundUnderT dd blockWidth*blockHeight dup ?
backGroundTexturesTable dd backgroundT
 
buf   dd ?

sandDynamicN db 'SandDynamic.png',0
stoneN db 'Stone.png',0
dirtyN db 'Dirty.png',0
divotN db 'Divot.png',0
divotValeryN db 'divotValery.png',0 
dioritN db 'Diorit.png',0
airN db 'Air.png',0
tropicWoodN db 'TropicWood.png',0
oakWoodN db 'OakWood.png',0
pineWoodN db 'PineWood.png',0
oakFoliageN db 'OakFoliage.png',0
pineFoliageN db 'PineFoliage.png',0
tropicFoliageN db 'TropicFoliage.png',0
cactusN db 'Cactus.png',0
cactusUpN db 'CactusUp.png',0

dioritT dd blockWidth*blockHeight dup 0
airT dd blockWidth*blockHeight dup 0
testStoneT dd blockWidth*blockHeight dup 0
stoneT dd blockWidth*blockHeight dup 0
dirtyT dd blockWidth*blockHeight dup 0
divotT dd blockWidth*blockHeight dup 0
divotValeryT dd blockWidth*blockHeight dup 0 
sandDynamicT dd blockWidth*blockHeight dup 0
tropicWoodT dd blockWidth*blockHeight dup 0
oakWoodT dd blockWidth*blockHeight dup 0
pineWoodT dd blockWidth*blockHeight dup 0
oakFoliageT dd blockWidth*blockHeight dup 0 
pineFoliageT dd blockWidth*blockHeight dup 0
tropicFoliageT dd blockWidth*blockHeight dup 0
cactusT dd blockWidth*blockHeight dup 0
cactusUpT dd blockWidth*blockHeight dup 0
  