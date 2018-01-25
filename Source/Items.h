#ifndef _ITEMS
#define _ITEMS

#include "Globals.h"
#include "Remap.h"

//---------------------------------------------------------

#define									kSpeedAttribute						1
#define									kPowerAttribute						2
#define									kDurabilityAttribute				3
#define									kSilverAttribute					4
#define									kPoisonAttribute					5

//---------------------------------------------------------

#define									kKatana										1
#define									kGrayWizardHat						2
#define									kFishingPole							3
#define									kGrayShirt							  4
#define									kLightBlueShirt						5
#define									kRedShirt							  	6
#define									kBlueShirt							  7
#define									kBrownShirt							  8
#define									KBlackShirt							  9
#define									kGreenShirt							  10
#define									kPurpleShirt							11
#define									kGrayPants							  12
#define									kLightBluePants						13
#define									kRedPants							  	14
#define									kBluePants							  15
#define									kBrownPants							  16
#define									KBlackPants							  17
#define									kGreenPants							  18
#define									kPurplePants							19
#define									kGrayRobe							  	20
#define									kLightBlueRobe						21
#define									kRedRobe							  	22
#define									kBlueRobe							  	23
#define									kBrownRobe							  24
#define									KBlackRobe							  25
#define									kGreenRobe							  26
#define									kPurpleRobe								27
#define									kLongSword								28
#define									kBroadSword								29
#define									kScimitar		 							30
#define									kKnife										31
#define									kRedWizardHat						  32
#define									kStave										33
#define									kSickle										34
#define									kClub											35
#define									kFlail										36
#define									kShortBow									37
#define									kBattleAxe								38
#define									kHandAxe									39
#define									kGlaive										40
#define									kSpear										41
#define									kWarHammer								42
#define									kMace											43
#define									kMiningAxe								44
#define									kWoodworkingTools					45
#define									kTinkeringTools						46
#define									kLongBow    							47
#define									kBreastPlate							48
#define									kBlueWizardHat						49
#define									kBrownWizardHat						50
#define									kPurpleWizardHat					51
#define									kBrownBreastLeather				52
#define									kBrownLeatherLegs				  53
#define									kBrownLeatherArms					54
#define									kGreenWizardHat						55
#define									kLightBlueWizardHat				56
#define									kBlackBreastPlate					57
#define									kBreastChain							58
#define									kYellowShirt							59
#define									kYellowRobe								60
#define									kYellowPants							61
#define									kLegsPlate								62
#define									kBlackLegsPlate						63
#define									kMedalShield							64
#define									kBlackWizardHat						65
#define									kLightBlueBoots						66
#define									kRedBoots							  	67
#define									kBlueBoots							  68
#define									kBrownBoots							  69
#define									kBlackBoots							  70
#define									kGreenBoots							  71
#define									kPurpleBoots							72
#define									kYellowBoots							73
#define									kOpenPlateHelmet					74
#define									kPlateGauntlets						75
#define									kBlacksmithHammer					76
#define									kCarvingKnife							77
#define									kSlottedPlateHelmet				78
#define									kClosedPlateHelmet				79
#define									kVikingHelmet					    80
#define									kChainMailHelmet					81
#define									kWoodenShield					    82
#define									kChainLegs								83
#define									kChainArms								84
#define									kPlateArms								85
#define									kBlackPlateArms						86
#define									kChainGauntlets						87
#define									kBlackPlateGauntlets			88
#define									kBlackOpenPlateHelmet     89
#define									kBlackSlottedPlateHelmet	90
#define									kBlackClosedPlateHelmet		91
#define									kBlackVikingHelmet				92
#define									kYellowWizardHat				  93
#define									kKatanaDurability					94
#define									kKatanaPower							95
#define									kKatanaSpeed							96
#define									kKatanaSilver							97
#define									kKatanaPoison							98
#define									kLongSwordDurability			99
#define									kLongSwordPower						100
#define									kLongSwordSpeed						101
#define									kLongSwordSilver					102
#define									kLongSwordPoison					103
#define									kBroadSwordDurability			104
#define									kBroadSwordPower					105
#define									kBroadSwordSpeed					106
#define									kBroadSwordSilver					107
#define									kBroadSwordPoison					108
#define									kScimitarDurability			  109
#define									kScimitarPower					  110
#define									kScimitarSpeed					  111
#define									kScimitarSilver					  112
#define									kScimitarPoison					  113
#define									kKnifeDurability			    114
#define									kKnifePower					      115
#define									kKnifeSpeed					      116
#define									kKnifeSilver					    117
#define									kKnifePoison					    118
#define									kStaveDurability			    119
#define									kStavePower					      120
#define									kStaveSpeed					      121
#define									kSickleDurability			    122
#define									kSicklePower					    123
#define									kSickleSpeed					    124
#define									kSickleSilver					    125
#define									kSicklePoison					    126
#define									kClubDurability			      127
#define									kClubPower					      128
#define									kClubSpeed					      129
#define									kFlailDurability			    130
#define									kFlailPower					      131
#define									kFlailSpeed					      132
#define									kShortBowDurability			  133
#define									kShortBowPower					  134
#define									kShortBowSpeed					  135
#define									kBattleAxeDurability			136
#define									kBattleAxePower					  137
#define									kBattleAxeSpeed					  138
#define									kBattleAxeSilver					139
#define									kBattleAxePoison					140
#define									kHandAxeDurability			  141
#define									kHandAxePower					    142
#define									kHandAxeSpeed					    143
#define									kHandAxeSilver					  144
#define									kShears					          145
#define									kHandAxePoison					  146
#define									kGlaiveDurability			    147
#define									kGlaivePower					    148
#define									kGlaiveSpeed					    149
#define									kGlaiveSilver					    150
#define									kGlaivePoison					    151
#define									kSpearDurability			    152
#define									kSpearPower					      153
#define									kSpearSpeed					      154
#define									kSpearSilver					    155
#define									kSpearPoison					    156
#define									kMaceDurability			      157
#define									kMacePower					      158
#define									kMaceSpeed					      159
#define									kWarHammerDurability			160
#define									kWarHammerPower					  161
#define									kWarHammerSpeed					  162
#define									kLongBowDurability			  163
#define									kLongBowPower					    164
#define									kLongBowSpeed					    165
#define									kHolyDagger								166
#define									kGMRobe								    167
#define									kSextant							    168
#define									kBloodDagger							169

#define									kGrayRobeProtection				170
#define									kLightBlueRobeProtection	171
#define									kRedRobeProtection				172
#define									kBlueRobeProtection				173
#define									kBrownRobeProtection			174
#define									kBlackRobeProtection			175
#define									kGreenRobeProtection			176
#define									kPurpleRobeProtection			177
#define									kYellowRobeProtection			178

#define									kGrayRobeResistance				179
#define									kLightBlueRobeResistance	180
#define									kRedRobeResistance				181
#define									kBlueRobeResistance				182
#define									kBrownRobeResistance			183
#define									KBlackRobeResistance			184
#define									kGreenRobeResistance			185
#define									kPurpleRobeResistance			186
#define									kYellowRobeResistance			187

#define									kDwarvenHandAxe						188
#define									kLobsterPole							189
#define									kEelPole							    190
#define									kAmberAxe						      191
#define									kCoalAxe                  192
#define									kFireSword                193
#define									kCentaurMiningAxe         194
#define									kFrostAxe                 195
#define									kNeptunesTrident          196
#define									kRoyalRobe								197

#define									kGrayJesterHat 					  198
#define									kLightBlueJesterHat       199
#define									kRedJesterHat             200
#define									kBlueJesterHat            201
#define									kBrownJesterHat           202
#define									kBlackJesterHat           203
#define									kGreenJesterHat           204
#define									kPurpleJesterHat          205
#define									kYellowJesterHat          206

#define									kGoldCrown                207

#define									kBurningPlateBreast					208
#define									kBurningPlateLegs					209
#define									kBurningPlateArms					210
#define									kBurningPlateHelmet					211
#define									kBurningPlateGauntlets				212
#define									kBurningMetalShield					213

#define									kBlackMetalShield					214
#define									kWhiteRobe							215

#define									kGMStaff							216
//Dave
#define									kGrayBoots							217
#define									kBlackBreastLeather					218
#define									kGreenBreastLeather					219
#define									kPurpleBreastLeather				220
#define									kYellowBreastLeather				221
#define									kBigJackKatana						223
#define									kFirstCrown							224
#define									kSecondCrown						225
#define									kThirdCrown							226
#define									kBluePlateHelmet					227
#define									kBluePlateBreast					228
#define									kBluePlateLegs						229
#define									kBluePlateArms						230
#define									kBluePlateGauntlets					231
#define									kRuneSword							232
#define									kRoyalSceptre						233

//----------------

#define									kLoafOfBread							256
#define									kEmptyBottle							257
#define									kLog											258
#define									kBandage							  	259
#define									kTrout										260
#define									kCookedSteak							261
#define									kRoastedGaperStalk				262
#define									kFerriteOre								263
#define									kFishSteak								264
#define									kEelPie										265
#define									kLobsterPie								266
#define									kDriedTrout								267
#define									kApplePie									268
#define									kGingerBread							269
#define									kOatMealCake							270
#define									kLobsterSoup							271
#define									kLobster									272
#define									kPackOfNails							273
#define									kPoison					          274
#define									kDragonTooth					    275
#define									kPoisonFieldSpell					276
#define									kParalyzeFieldSpell				277

#define									kBoard										278
#define									kArrow										279
#define									kBolt											280

#define									kLesserHealPotion						282
#define									kManaPotion								283
#define									kDiamond									284
#define									kRuby											285
#define									kCrystalRing							286
#define									kPlatinumRing							287
#define									kGoldRing									288
#define									kSilverRing								289
#define									kShaft										290
#define									kFeather									291
#define									kRollOfCloth							292
#define									kLeatherHide							293
#define									kGinseng									294			// purify
#define									kLotusFlower							295			// healing, meditation
#define									kMandrakeRoot							296			// magic protection
#define									kAmber									  297			// luck
#define									kGarlic										298			// 
#define									kVolcanicAsh							299			// 
#define									kPoisonGland							300			// 
#define									kCoal									    301			// poison
#define									kElectricEel							302			// 
#define									kLightBlueDye							303
#define									kRedDye										304
#define									kBlueDye									305
#define									kBrownDye									306
#define									kBlackDye									307
#define									kGreenDye									308
#define									kPurpleDye								309
#define									kYellowDye								310
#define									kFurSkin									311			// 
#define									kLesserHealSpell					312			// done
#define									kLesserDetoxifySpell			313			// 
#define									kMagicDisarmSpell					314			// 
#define									kHealPetSpell					315			// 
#define									kRevealSpell							316			// 
#define									kCreateFoodSpell					317			// 
#define									kNightVisionSpell					318			// 
#define									kClumsySpell							319			// 
#define									kSimpleMindSpell					320			// 
#define									kWeakenSpell							321			// 
#define									kAgilitySpell							322			// 
#define									kElevateMindSpell					323			// 
#define									kMagicTrapSpell						324			// 
#define									kStrengthSpell						325			// 
#define									kSummonPetSpell						326			// 
#define									kInvisibilitySpell				327			// 
#define									kGreaterHealSpell					328			// 
#define									kGreaterDetoxifySpell			329			// 
#define									kPoisonSpell							330			// 
#define									kLightningBoltSpell				331			// done
#define									kReflectiveArmorSpell			332			// 
#define									kMagicReflectionSpell			333			// 
#define									kDrainManaSpell						334			// 
#define									kFireFieldSpell						335			// done
#define									kTeleportSpell						336			// 
#define									kDisorientationSpell			337			// 
#define									kParalyzeSpell						338			// 
#define									kExplosionSpell						339			// 
#define									kSummonCreatureSpell			340			// 
#define									kRaft						          341			// 
#define									kLesserDetoxifyPotion			342
#define									kStrengthPotion						343
#define									kAgilityPotion						344
#define									kElevateMindPotion				345
#define									kNightVisionPotion				346
#define									kLock						          347
#define									kTurtleShell							348
#define									kSilverNecklace						349
#define									kLeatherBelt							350
#define									kMugOfAle					        351
#define									kRawSteak									352
#define									kBundleOfWool							353
#define									kTrap					            354
#define									kApple					          355
#define									kBanana					          356
#define									kOrbOfSeeing					    357
#define									kRedRose					        358
#define									kYellowRose					      359
#define									kBlackRose					      360
#define									kGreaterHealPotion				361
#define									kFullHealPotion						362
#define									kGreaterDetoxifyPotion		363
#define									kFullDetoxifyPotion				364
#define									kInvisibilityPotion				365
#define									kNourishPotion						366
#define									kFullDetoxifySpell				367			// 
#define									kFullHealSpell						368			// done
#define									kResurrectSpell						369			// done
#define									kTurnUndeadSpell					370			// 
#define									kRatTail									371			// 
#define									kIngot									  372			// 
#define									kKindling                 373
#define									kProvocationSpell					374			// 
#define									kCalmSpell					      375			// 
#define									kScorpionStinger					376			// 
#define									kQueensRuby								377
#define									kSkull								    378
#define									kDragonScale							379
#define									kGaperStalk							  380
#define									kSackOfSpice							381
#define									kBoiledLobster						382
#define									kBagOfFlour								383
#define									kBananaPie								384
#define									kAncientTome							385
#define									kIceCrystal							  386

#define									kGreenCrystal							387
#define									kRedCrystal							  388
#define									kBlueCrystal							389
#define									kPurpleCrystal						390
#define									kOrangeCrystal						391
#define									kYellowCrystal						392

#define									kBrigandHead						  393
#define									kSorcererHead						  394
#define									kPirateHead						    395
#define									kBrackenSeed						  396

#define									kGoldMirror						    397
#define									kBookOfTheDead						398
#define									kMagicRune						    399
#define									kSilverDragonClaw					400
#define									kRubyHeart						    401
#define									kBookOfTheStars						402
#define									kKingsSeal						    403
#define									kCrystalFlask						  404
#define									kSilverLeaf						    405
#define									kTomeOfTheDamned					406
#define									kTomeOfLife						    407
#define									kGoldenFleece						  408

#define									kFireCrystal							409
#define									kCandy							      410

#define									kRedEgg							      411
#define									kPurpleEgg							  412
#define									kGreenEgg							    413
#define									kBlueEgg							    414

#define									kGuildHallDeed						415
#define									kBlackCrystal						  416

#define									kBlueFlag							    417
#define									kRedFlag							    418
#define									kGreenFlag							  419
#define									kYellowFlag							  420

#define									kSpiderFang					      421			// 

#define									kDragonStoneRing				  422
#define									kElementalRing						423
#define									kPoisonRing								424
#define									kNeptunesRing							425
#define									kHadesRing								426
#define									kManaRing								  427
#define									kDruidRing								428
#define									kChaosRing								429
#define									kRingOfLife							430

#define									kCrystalSkull							431
#define									kKingsPortrait						432
#define									kPersianRug							  433
#define									kHouseDeed						    434

#define									kRemoveCurseSpell					435			// 

#define									kLesserHealRing						436
#define									kLightningBoltRing					437
#define									kGreaterHealRing					438
#define									k5ARRing							439
#define									k10ARRing							440
#define									k15ARRing							441
#define									kRemoveCurseRing					442

#define									kCurio1								443
#define									kCurio2								444
#define									kCurio3								445
#define									kCurioBig							446
#define									kCurioSmall							447
#define									kGoldenBox							448
#define									kBody								449

#define									kPureBlueCrystal					450
#define									kPureRedCrystal						451
#define									kPureYellowCrystal					452
#define									kPureOrangeCrystal					453
#define									kPureGreenCrystal					454
#define									kPurePurpleCrystal					455
#define									kPureFireCrystal					456
#define									kPureIceCrystal						457
#define									kPureBlackCrystal					458

#define									kSmallBag 							459
#define									kLockbox 							460
#define									kPapers 							461
#define									kMagicStone 						462
#define									kSmallKey 							463
#define									kMessage 							464
#define									kCrate 								465
#define									kVialOfBlood 						466
#define									kDocuments 							467
#define									kLetter 							468
#define									kRustyKey 							469
#define									kIronKey 							470
#define									kBronzeKey							471
#define									kGoldenBolt							472
#define									kSilverBolt 						473
#define									kDragonEggGreen	 					474
#define									kSnakeSkin 							475
#define									kSunstoneRing 						476
#define									kFlaskOfPerfume 					477
#define									kLittleStatue 						478
#define									kLockOfHair 						479
#define									kJar 								480
#define									kParchment							481
#define									kBoneKnife 							482
#define									kPearlNecklace						483
#define									kStatueOfElara	 					484
#define									kMysticSigil 						485
#define									kArcaneGrimoire 					486
#define									kSilverChalice 						487
#define									kInfernalTome 						488
#define									kEmeraldPrism						489
#define									kSapphirePrism						490
#define									kAmethystPrism 						491
#define									kRubyPrism 							492
#define									kTopazPrism 						493
#define									kDiamondPrism 						494
#define									kGoldCandelabrum 					495
#define									kBookofMoons 						496
#define									kBlinkHoundFur 						497
#define									kZionidicCrystal 					498
#define									kVermaStone 						499
#define									kSpiralCharm 						500
#define									kGoldThread 						501
#define									kVialOfCentaurBlood 				502
//#define									kRoyalSceptre 						503
#define									kMirithToken 						504
#define									kMaraliToken 						505
#define									kAndrisToken 						506
#define									kMayorSeal	 						507
#define									kSenateSeal 						508
#define									kCommanderSeal	 					509
#define									kStoreToken							510
#define									kIvoryTusk							511
#define									kShevPortrait						512
#define									kDragonEggGold	 					513
#define									kDragonEggBlack 					514
#define									kDragonEggRed	 					515
#define									kDragonEggBlue	 					516
#define									kDragonEggPurple 					517
#define									kKeyBow								518
#define									kKeyCollar							519
#define									kKeyShank							520
#define									kKeyShoulder						521
#define									kKeyStem							522
#define									kKeyBit								523
#define									kOpportunityPotion					524
#define									kSickHorse							525
#define									kHorse								526
#define									kFastHorse							527
#define									kVeryFastHorse						528
#define									kMagicMount1						529
#define									kMagicMount2						530
#define									kMagicMount3						531
#define									kMagicMount4						532
#define									kGMHorse							533
#define									kEtheriteWood						534
#define									kEtheriteFerrite					535
#define									kEtherite							536
#define									kInfusedGreen						537
#define									kInfusedRed							538
#define									kInfusedBlue						539
#define									kInfusedYellow						540
#define									kInfusedOrange						541
#define									kInfusedPurple						542
#define									kInfusedBlack						543
#define									kInfusedPGreen						544
#define									kInfusedPRed						545
#define									kInfusedPBlue						546
#define									kInfusedPYellow						547
#define									kInfusedPOrange						548
#define									kInfusedPPurple						549
#define									kInfusedPBlack						550
#define									kInfusedFire						551
#define									kInfusedIce							552
#define									kInfusedCS							553
#define									kGaperEye							554
#define									kElementalDust						555
#define									kAcidicGland						556
#define									kBrackenRoots						557
#define									kBoneMageStaff						558
#define									kHarpieBeak							559
#define									kLizardScale						560
#define									kBatWing							561
#define									kSpiderLeg							562
#define									kSnailSlime							563
#define									kZombieBrain						564
#define									kRustGland							565
#define									kGiantHeart							566
#define									kSnakeEgg							567
#define									kAntMandibles						568
#define									kCubeGoo							569
#define									kTuskerTail							570
#define									kAncientRune						571
#define									kNeviaFlower						572
#define									kSmallMoonFragment					573
#define									kMerefolkHair						574
#define									kRustyNecklace						575
#define									kPureCrystalRing					576
#define									kEnchantedCloth						577
#define									kDiamondDust						578
#define									kDiscretionRing						579
#define									kCharmOfUndeath						580
#define									kVialOfAcid							590
#define									kRingOfStrength						591
#define									kRingOfStealth						592
#define									kCharmOfSwiftness					593
#define									kCharmOfLoyalty						594
#define									kCharmOfPurity						595
#define									kRingOfBruteStrength				596
#define									kLesserManaRing						597
#define									kRingOfShadows						598
#define									kEssenceOfPower						599
#define									kEssenceOfDurability				600
#define									kEssenceOfSpeed						601
#define									kBlueTicket							602
#define 								kBloodAmulet 						603
#define 								kBloodStainedMoonstone 				604
#define 								kBloodTome 							605
#define 								kCrystalNecklace 					606
#define 								kMoonstoneRing					 	607
#define 								kJournal 							608
#define 								kTornNote 							609
#define									kCrystalSkullPure					610
#define 								kCrystalChalice 					611
#define 								kBloodyJournal 						612
#define 								kBloodyNote 						613
#define 								kSkeletonKey 						614
#define 								kNorthStone 						615
#define 								kSouthStone 						616
#define 								kEastStone 							617
#define 								kWestStone 							618
#define 								kThread								619
#define 								kBead 								620
#define 								kBrokenNecklace 					621
#define 								kRuinedBook 						622
#define 								kBloodStainedCloth 					623
#define 								kMuddyBoots 						624
#define 								kOrnateStaff 						625
#define 								kMoldyBook 							626
#define 								kLogbook 							627
#define 								kRegister 							628
#define 								kShreddedTome 						629
#define 								kTornPage 							630
#define 								kGlowingBottle 						631
#define 								kCrystalDragonStatue 				632
#define 								kSilverFigurine 					633
#define 								kStarDiamond 						634
#define 								kShadowedOrb 						635
#define 								kSilverTippedQuill 					636
#define 								kCrystalGoddessCharm 				637
#define 								kJeweledHairPin 					638
#define 								kMercenaryToken 					639
#define 								kCrystalToken						640
#define 								kDecorativeVase						641
#define 								kWinningTicket						642
#define 								kBettingSlip						643
#define 								kLeaderPin							644
#define 								kOldSword							645
#define 								kRedWine							646
#define 								kCopperMug							647
#define 								kCopperLantern						648
#define 								kMirithPaleAle						649
#define 								kMaraliWhiskey						650
#define 								kAndrisChampagne					651
#define 								kBuckeyeBeer						652
#define 								kLerilinCremeale					653
#define 								kRedNatesRotgutRum					654
#define									kGoldenScale  						655
#define									kWoodenTrinket  					656
#define									kPetrifiedGaperEye  				657
#define									kSilkPouch  						658
#define									kOrnateWalkingStick  				659
#define									kGoddessCharm 						660
#define									kCrystalSpiderWeb  					661
#define									kWoodenFlute  						662
#define									kLute  								663
#define									kSilverFlute  						664
#define									kGoldLadle  						665
#define									kCharStatue							666
#define									kDarkRuby  							667
#define									kMortarandPestle  					668
#define									kBookofFairyTales  					669
#define									kJeweledDagger						670
#define									kGrayDye							671
#define									kGarlicBread						672
#define									kSheepsMilk							673
#define									kSteakFlambe						674
#define									kSushi								675
#define									kRedOmelette						676
#define									kPurpleOmelette						677
#define									kWaterBottle						678
#define									kRumCake							679
#define									kPipe								680
#define									kMushroom							681
#define									kCandiedApple						682
#define									kPizza								683
#define									kBlueRose							684
#define									kMead								685
#define									kBlueMoonGin						686
#define									kHayateSake							687
#define									kPumpkinPie							688
#define									kPortGastMoonshine					689
#define									kFoehanVodka						690
#define									kWizardToken						691
#define									kDruidToken							692
#define									kClericToken						693
#define									kFighterToken						694
#define									kRangerToken						695
#define									kRogueToken							696
//#define									kJinkSword							697
#define									kBlackFerrite						698
#define									kBlackIngot							699
//#define									kBreastPlate						700
//#define									kLegsPlate							701
//#define									kOpenPlateHelmet					702
//#define									kPlateArms							703
//#define									kPlateGauntlets						704
//#define									kSlottedPlateHelmet					705
//#define									kClosedPlateHelmet					706
//#define									kVikingHelmet					    707

#define									kZombieBrainStew					708
#define									kMandrakeCookie						709
#define									kBlackEtherite						710
#define									kJinkDoll							711
#define									kVoodooDoll							712
#define									kNPCPrivateerHead					713
#define									kTuna								714
#define									kSeaBass							715
#define									kCopperSpoon						716
#define									kSilverButton						717
#define									kTinPlate							718
#define									kSakeSeaBass						719
#define									kWhiteWine							720
#define									kCrystalBrooch						721
#define									kTornCloth							722
#define									kBookOfBlood						723
#define									kRustySextant						724
#define									kAmuletOfStars						725
#define									kAmuletOfSky						726
#define									kAmuletOfTheMoon					727
#define									kCeruleanDiamond					728
#define									kJeweledRing						729
#define									kBrokenRing							730
#define									kDriftWoodFigurine					731
#define									kSilverShell						732
#define									kTinySeaShell						733
#define									kReed								734
#define									kKelp								735
#define									kCharmOfTheSea						736
#define									kOrnateShellHorn					737
#define									kBraidedKelpCharm					738
#define									kBookOfSky							739
//#define									kGrayBoots							740
#define									kGreaterSummonCreatureSpell			740
#define									kStuffedGiantHeart					741
#define									kEggNog								742
#define									kAndrisBananaCake					743
#define									kMorgansGingerbread					744
#define									kWizardReward						745
#define									kClericReward						746
#define									kDruidReward						747
#define									kFighterReward						748
#define									kRangerReward						749
#define									kRogueReward						750
#define									kPixieDust							751
#define									kCopperCoin							752
#define									kKingFigurine						753
#define									kQueenFigurine						754
#define									kBishopFigurine						755
#define									kKnightFigurine						756
#define									kRookFigurine						757
#define									kPawnFigurine						758
#define									kSilverCoin							759
#define									kRedToken							760
#define									kBlackToken							761
#define									kSilverToken						762
#define									kGrainOfRedSand						763
#define									kCandle								764
#define									kBirthdayCake						765
#define									kBlackEgg							766
#define									kGohoranChocolate					767
#define									kSpiderEgg							768
#define									kBearSkin							769
#define									kSharkFin							770
#define									kSharkFinSoup						771
#define									kTrollHouseCookie					772
#define									kGoldMedal							773
#define									kSilverMedal						774
#define									kBronzeMedal						775
#define									kNeedle								776
#define									kSLToken							777
#define									kJadeNecklace						778
#define									kJadePin							779
#define									kJadeStatue							780
#define									kJadeBox							781
#define									kClenseSpell						782
#define									kIridescentToken					783
#define									kBrigandInsignia					784
#define									kBlueMetal							785
#define									kRingOfInsight						786
#define									kPortraitLady						787
#define									kPortraitTirana						788
#define									kElixirHeart						789
#define									kElixirDuh							790
#define									kElixirZimm							791
#define									kElixirKapow						792
#define									kElixirTingle						793
#define									kOrangeRobe							794
#define									kOrangeDye							795
#define									kPurpleRose					        796
#define									kTomeoftheLost				        797


//#define									kBlackPlateArms						750
//#define									kBlackBreastPlate					751
//#define									kBlackLegsPlate						752
//#define									kBlackPlateGauntlets				753
//#define									kBlackOpenPlateHelmet     			754
//#define									kBlackSlottedPlateHelmet			755
//#define									kBlackClosedPlateHelmet				756
//#define									kBlackVikingHelmet					757


//#define									kBluePlateHelmet					757
//#define									kBluePlateBreast					758
//#define									kBluePlateLegs						759
//#define									kBluePlateArms						760
//#define									kBluePlateGauntlets					761

#define									kFireballSpell						1000
#define									kMagicMissileSpell					1001
#define									kFireblastSpell						1002
#define									kFireblast3Spell					1003
#define									kFireblast4Spell					1004
#define									kFireblast5Spell					1005
#define									kMassHealSpell						1006
#define									kDisorientFieldSpell				1007
#define									kOpportunitySpell					1008
#define									kSummonBlackDracoSpell				1009
#define									kSummonSkelsSpell					1010
#define									kChaosSpell							1011
#define									kSummonRedDracoSpell				1012
#define									kSummonIceDracoSpell				1013
#define									kSummonGreenDracoSpell				1014
#define									kNatureSplashSpell					1015
#define									kIceSplashSpell						1016
#define									kCreatureHealSpell					1017
#define									kRootAttackSpell					1018
#define									kShapeshiftLesserSpell				1019
#define									kShapeshiftSpell					1020
#define									kShapeshiftGreaterSpell				1021
#define									kShapeshiftAllSpell					1022
#define									kShapeshiftDragonSpell				1023
//
#define									kFirstFieldSpell					1500
//
#define									kChaosFieldSpell					1501
#define									kFireblastFieldSpell				1502
#define									kIceFieldSpell						1503
#define									kNatureFieldSpell					1504			
//
#define									kLastFieldSpell						1504
//

//rogues
#define									kAttackInstantStrike				2000
#define									kAttackBackstab						2001
#define									kAttackVial							2002
#define									kAttackSting						2003
#define									kAttackDecimate						2004

//rangers
#define									kAttackDoubleShot					2100
#define									kAttackRangerSwiftness				2101
#define									kAttackEscapeMove					2102
#define									kAttackAimedShot					2103
#define									kAttackGrapplingShot				2104

//fighters
#define									kAttackTaunt						2200
#define									kAttackDoubleStrike					2201
#define									kAttackQuickDraw					2202
#define									kAttackLesserDefense				2203
#define									kAttackKnockback					2204
#define									kAttackRage							2205
#define									kAttackBoomingBlow					2206
#define									kAttackGreaterDefense				2207

#define									kMaxItemsUsed					  	5500 // Dave 11-23-2012 --- 4999 

//---------------------------------------------------------

#define									kMiscType									1
#define									kRawMeatType							2
#define									kCookedMeatType						3
#define									kHatType									4
#define									kClothingType							5
#define									kRobeType									6
#define									kShirtType								7
#define									kPantsType								8
#define									kMiningToolType						9
#define									kWoodToolType						  10
#define									kTinkerToolType						11

#define									kMiscWeaponType						12
#define									kAxeWeaponType						13

#define									kFishingToolType					14
#define									kPotionType								15

#define									kArmorType								16
#define									kLeatherArmorType				  17
#define									kBootsType								18

#define									kReagentType							19
#define									kSpellType								20

#define									kWaistType								21
#define									kBlacksmithToolType		   			 22
#define									kCarvingToolType		   		   23
#define									kScrollType		          			  24

#define									kBeverageType						  25

#define									kArrowWeaponType					26
#define									kBoltWeaponType						27
#define									kWizardHatType						28
#define									kLumberjackToolType				29

#define									kRawVegType								30
#define									kCookedVegType						31
#define									kNavigationToolType				32
#define									kEmptyHands				        33
#define									kSewingToolType				    34

#define									kMiscTailorType				    35
#define									kOrbType				          36
#define									kQuestType				        37
#define									kGMRobeType								38
#define									kCrystalType							39  // mod 8/4
#define									kRoyalRobeType					  40
#define									kJesterHatType						41
#define									kCrownType						    42
#define									kMagicRobeType						43
#define									kLesserRingType						44
#define									kArmorRingType						45
#define									kDyeType							46
#define									kCurioThirdType						47
#define									kCurioType							48
#define									kCurioFragmentType					49
#define									kMountType							50
#define									kInfusedCrystalType					51
#define									kEtheriteType						52
#define									kSpecialAttackType					53
#define									kWhiteRobeType						54
#define									kCharmType							55
#define									kAnyWeaponType						56
#define									kMagicWeaponType					57
//Dave
#define									kBeverageType2						58
#define									kBeverageType3						59
#define									kToolType							60
#define									kBeverageType4						61
#define									kRewardType							62
#define 								kElixirType1						63
#define 								kElixirType2						64
#define 								kElixirType3						65
#define 								kElixirType4						66
#define 								kElixirType5						67

#define									kAllCategories						99
//---------------------------------------------------------

#define									kCantArm								0
#define									kRightHand								1
#define									kHead									2
#define									kTorso									3
#define									kLegs									4
#define									kLeftHand								5
#define									kFeet									6
#define									kArms									7
#define									kRightFinger							8
#define									kLeftFinger								9
#define									kNeck									10
#define									kHands									11
#define									kWaist									12
#define									kMount									13
#define									kBothHands								14

#define									kGray									0
#define									kLightBlue								1
#define									kRed									2
#define									kBlue									3
#define									kBrown									4
#define									kBlack									5
#define									kGreen									6
#define									kPurple									7
#define									kYellow									8
#define									kNoColor								9
#define									kOrange									10
//----------------------------------------------------------

typedef struct itemType {
		UInt8									useLocation;
		TBoolean								twoHandItem;
		UInt8									itemType;
		UInt8									itemColor;		
		UInt16									pictID;
		UInt8									weight;
		UInt16									price;
		UInt8									armorRating;
		UInt8                 					dexPenalty;
		UInt8									weaponRange;
		UInt16									weaponSpeed;
		UInt8									weaponDamage;
		UInt8									durability;
		UInt8									makeSkill;
		UInt8									makeCount;
		UInt8									makeSkillLevel;
		UInt8                 					makeMana;
		UInt8									ingredientCount[5];
		UInt16									ingredientID[5];
		UInt8									useSkill;
		TBoolean              					blessed;
		long									currentCount;
		long									desiredCount;
		TBoolean              					silver;
		int                   					parent;
		TBoolean              					magical;
		TBoolean              					poison;
		TBoolean              					metal;
		int										spell; //Allows wearer to cast this spell (no reagent or mp used)
		//UInt8									itemCount;
	} itemType;	

//----------------------------------------------------------

extern 			itemType						itemList						[kMaxItems];
extern 			char 								makeSuccessMessage	[kStringLength];
extern 			char 								makeFailedMessage		[kStringLength];
//Dave
//extern creatureType									creature					[kMaxCreatures];

void 				it_InitializeItems				(void); // server
void 				it_CreateItem(int item, int use, int type, int pictID, int weight, int price, int color, int param1, int param2, int parent); // server
void 				it_CreateWeapon(int item, int use, int type, int pictID, int weight, int price, int range, int speed, int damage, int durability, int skill, long count, int parent, TBoolean metal); // server
void        it_CreateMagicWeapon(int item, int parent, long count, int magic); // server
TBoolean 		it_IsWeapon(int id);
TBoolean 		it_IsArmor(int id, TBoolean magic);
TBoolean    it_IsMetalArmor(int id);
void 				it_CalculateArmorRating(int i);
void				it_Ingredients(int item, int skill, int skillLevel, int c1, int i1, int c2, int i2, int c3, int i3, int c4, int i4, int c5, int i5, int count);
TBoolean 		it_CanMake(int item, int skill, int skillLevel);
void 				it_MakeItem(int i, int item);
void 				it_TryToMakeItem(int item);
void 				it_FailedMake(int i); // server
void 				it_SuccessfulMake(int i); // server
TBoolean 		it_IsClothing(int id);
TBoolean 		in_ItemIsUsable(int item);	// client
void        it_MagicLoreSuccess(int item);	// client
int         it_GetRare(int level); // server
TBoolean    it_HaveIngredients(int item);
void        it_TryToRepairItem(int slot);	// client
UInt8		it_GetMountSpeed(int item);
TBoolean	it_IsSpecialItem(int item, int count);
#endif