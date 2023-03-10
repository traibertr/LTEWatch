SamacSys ECAD Model
1033636/267059/2.49/6/3/Integrated Circuit

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "c20"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Ellipse)  (shapeWidth 0.2) (shapeHeight 0.2))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "BGA6C40P2X3_88X123X62" (originalName "BGA6C40P2X3_88X123X62")
		(multiLayer
			(pad (padNum 1) (padStyleRef c20) (pt -0.2, 0.4) (rotation 90))
			(pad (padNum 2) (padStyleRef c20) (pt 0.2, 0.4) (rotation 90))
			(pad (padNum 3) (padStyleRef c20) (pt -0.2, 0) (rotation 90))
			(pad (padNum 4) (padStyleRef c20) (pt 0.2, 0) (rotation 90))
			(pad (padNum 5) (padStyleRef c20) (pt -0.2, -0.4) (rotation 90))
			(pad (padNum 6) (padStyleRef c20) (pt 0.2, -0.4) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.457 1.628) (pt 1.457 1.628) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.457 1.628) (pt 1.457 -1.628) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.457 -1.628) (pt -1.457 -1.628) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.457 -1.628) (pt -1.457 1.628) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.442 0.613) (pt 0.442 0.613) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.442 0.613) (pt 0.442 -0.613) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.442 -0.613) (pt -0.442 -0.613) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.442 -0.613) (pt -0.442 0.613) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.442 0.299) (pt -0.128 0.613) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.2 0.85) (pt 0.65 0.85) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0.65 0.85) (pt 0.65 -0.85) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0.65 -0.85) (pt -0.65 -0.85) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.65 -0.85) (pt -0.65 0.4) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.65 0.4) (pt -0.2 0.85) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -0.65, 0.85) (radius 0.1) (startAngle 0.0) (sweepAngle 0.0) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -0.65, 0.85) (radius 0.1) (startAngle 180.0) (sweepAngle 180.0) (width 0.2))
		)
	)
	(symbolDef "TPS610992YFFT" (originalName "TPS610992YFFT")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 4) (pt 1100 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 870 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 5) (pt 1100 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 870 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 6) (pt 1100 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 870 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 900 mils 100 mils) (width 6 mils))
		(line (pt 900 mils 100 mils) (pt 900 mils -300 mils) (width 6 mils))
		(line (pt 900 mils -300 mils) (pt 200 mils -300 mils) (width 6 mils))
		(line (pt 200 mils -300 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 950 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "TPS610992YFFT" (originalName "TPS610992YFFT") (compHeader (numPins 6) (numParts 1) (refDesPrefix IC)
		)
		(compPin "A1" (pinName "VIN") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "A2" (pinName "GND") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B1" (pinName "SW") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B2" (pinName "VOUT") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "C1" (pinName "EN") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "C2" (pinName "FB") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "TPS610992YFFT"))
		(attachedPattern (patternNum 1) (patternName "BGA6C40P2X3_88X123X62")
			(numPads 6)
			(padPinMap
				(padNum 1) (compPinRef "A1")
				(padNum 2) (compPinRef "A2")
				(padNum 3) (compPinRef "B1")
				(padNum 4) (compPinRef "B2")
				(padNum 5) (compPinRef "C1")
				(padNum 6) (compPinRef "C2")
			)
		)
		(attr "Manufacturer_Name" "Texas Instruments")
		(attr "Manufacturer_Part_Number" "TPS610992YFFT")
		(attr "Mouser Part Number" "595-TPS610992YFFT")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Texas-Instruments/TPS610992YFFT?qs=1mbolxNpo8cSEKMr%252BGh28A%3D%3D")
		(attr "Arrow Part Number" "TPS610992YFFT")
		(attr "Arrow Price/Stock" "https://www.arrow.com/en/products/tps610992yfft/texas-instruments?region=nac")
		(attr "Mouser Testing Part Number" "")
		(attr "Mouser Testing Price/Stock" "")
		(attr "Description" "Conv DC-DC 0.7V to 5.5V Synchronous Step Down Single-Out 1.8V to 5.5V 0.3A 6-Pin DSBGA T/R")
		(attr "Datasheet Link" "http://www.ti.com/general/docs/suppproductinfo.tsp?distId=10&gotoUrl=http%3A%2F%2Fwww.ti.com%2Flit%2Fgpn%2Ftps61099")
		(attr "Height" "0.625 mm")
	)

)
