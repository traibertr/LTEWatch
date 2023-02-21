SamacSys ECAD Model
413652/267059/2.49/9/3/Integrated Circuit

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "c29"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Ellipse)  (shapeWidth 0.29) (shapeHeight 0.29))
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
	(patternDef "BGA9C50P3X3_162X158X62" (originalName "BGA9C50P3X3_162X158X62")
		(multiLayer
			(pad (padNum 1) (padStyleRef c29) (pt -0.5, 0.5) (rotation 90))
			(pad (padNum 2) (padStyleRef c29) (pt 0, 0.5) (rotation 90))
			(pad (padNum 3) (padStyleRef c29) (pt 0.5, 0.5) (rotation 90))
			(pad (padNum 4) (padStyleRef c29) (pt -0.5, 0) (rotation 90))
			(pad (padNum 5) (padStyleRef c29) (pt 0, 0) (rotation 90))
			(pad (padNum 6) (padStyleRef c29) (pt 0.5, 0) (rotation 90))
			(pad (padNum 7) (padStyleRef c29) (pt -0.5, -0.5) (rotation 90))
			(pad (padNum 8) (padStyleRef c29) (pt 0, -0.5) (rotation 90))
			(pad (padNum 9) (padStyleRef c29) (pt 0.5, -0.5) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.825 1.805) (pt 1.825 1.805) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.825 1.805) (pt 1.825 -1.805) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.825 -1.805) (pt -1.825 -1.805) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.825 -1.805) (pt -1.825 1.805) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.81 0.79) (pt 0.81 0.79) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.81 0.79) (pt 0.81 -0.79) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.81 -0.79) (pt -0.81 -0.79) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.81 -0.79) (pt -0.81 0.79) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.81 0.388) (pt -0.408 0.79) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.5 0.995) (pt 0.995 0.995) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0.995 0.995) (pt 0.995 -0.995) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0.995 -0.995) (pt -0.995 -0.995) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.995 -0.995) (pt -0.995 0.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.995 0.5) (pt -0.5 0.995) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -0.995, 0.995) (radius 0.1) (startAngle 0.0) (sweepAngle 0.0) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -0.995, 0.995) (radius 0.1) (startAngle 180.0) (sweepAngle 180.0) (width 0.2))
		)
	)
	(symbolDef "BQ27421YZFT-G1A" (originalName "BQ27421YZFT-G1A")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 4) (pt 0 mils -300 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -325 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 5) (pt 0 mils -400 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -425 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 6) (pt 1200 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 7) (pt 1200 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 8) (pt 1200 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 9) (pt 1200 mils -300 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 970 mils -325 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 1000 mils 100 mils) (width 6 mils))
		(line (pt 1000 mils 100 mils) (pt 1000 mils -500 mils) (width 6 mils))
		(line (pt 1000 mils -500 mils) (pt 200 mils -500 mils) (width 6 mils))
		(line (pt 200 mils -500 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 1050 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "BQ27421YZFT-G1A" (originalName "BQ27421YZFT-G1A") (compHeader (numPins 9) (numParts 1) (refDesPrefix IC)
		)
		(compPin "A1" (pinName "GPOUT") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "A2" (pinName "SDA") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "A3" (pinName "SCL") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B1" (pinName "BIN") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B2" (pinName "VSS_1") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B3" (pinName "VDD") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "C1" (pinName "VSS_2") (partNum 1) (symPinNum 7) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "C2" (pinName "SRX") (partNum 1) (symPinNum 8) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "C3" (pinName "BAT") (partNum 1) (symPinNum 9) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "BQ27421YZFT-G1A"))
		(attachedPattern (patternNum 1) (patternName "BGA9C50P3X3_162X158X62")
			(numPads 9)
			(padPinMap
				(padNum 1) (compPinRef "A1")
				(padNum 2) (compPinRef "A2")
				(padNum 3) (compPinRef "A3")
				(padNum 4) (compPinRef "B1")
				(padNum 5) (compPinRef "B2")
				(padNum 6) (compPinRef "B3")
				(padNum 7) (compPinRef "C1")
				(padNum 8) (compPinRef "C2")
				(padNum 9) (compPinRef "C3")
			)
		)
		(attr "Manufacturer_Name" "Texas Instruments")
		(attr "Manufacturer_Part_Number" "BQ27421YZFT-G1A")
		(attr "Mouser Part Number" "595-BQ27421YZFT-G1A")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Texas-Instruments/BQ27421YZFT-G1A?qs=%252BrH4t%252BeVZ2Mb4Ofcz0kOjQ%3D%3D")
		(attr "Arrow Part Number" "BQ27421YZFT-G1A")
		(attr "Arrow Price/Stock" "https://www.arrow.com/en/products/bq27421yzft-g1a/texas-instruments?region=nac")
		(attr "Mouser Testing Part Number" "")
		(attr "Mouser Testing Price/Stock" "")
		(attr "Description" "System-Side(tm) Fuel Gauge With Integrated Sense Resistor | Battery Gas Gauge")
		(attr "Datasheet Link" "http://www.ti.com/lit/ds/symlink/bq27421-g1.pdf")
		(attr "Height" "0.625 mm")
	)

)
