SamacSys ECAD Model
15991508/267059/2.49/4/3/Integrated Circuit

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "s16.5"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.165) (shapeHeight 0.165))
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
	(patternDef "TPS7A2128PYWDJ" (originalName "TPS7A2128PYWDJ")
		(multiLayer
			(pad (padNum 1) (padStyleRef s16.5) (pt -0.175, 0.175) (rotation 90))
			(pad (padNum 2) (padStyleRef s16.5) (pt 0.175, 0.175) (rotation 90))
			(pad (padNum 3) (padStyleRef s16.5) (pt -0.175, -0.175) (rotation 90))
			(pad (padNum 4) (padStyleRef s16.5) (pt 0.175, -0.175) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.314 0.314) (pt 0.314 0.314) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.314 0.314) (pt 0.314 -0.314) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.314 -0.314) (pt -0.314 -0.314) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.314 -0.314) (pt -0.314 0.314) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.314 1.314) (pt 1.314 1.314) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.314 1.314) (pt 1.314 -1.314) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.314 -1.314) (pt -1.314 -1.314) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.314 -1.314) (pt -1.314 1.314) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.8 0.2) (pt -0.8 0.2) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -0.75, 0.2) (radius 0.05) (startAngle 180.0) (sweepAngle 180.0) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.7 0.2) (pt -0.7 0.2) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -0.75, 0.2) (radius 0.05) (startAngle .0) (sweepAngle 180.0) (width 0.1))
		)
	)
	(symbolDef "TPS7A2128PYWDJ" (originalName "TPS7A2128PYWDJ")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 1000 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 770 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 4) (pt 1000 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 770 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 800 mils 100 mils) (width 6 mils))
		(line (pt 800 mils 100 mils) (pt 800 mils -200 mils) (width 6 mils))
		(line (pt 800 mils -200 mils) (pt 200 mils -200 mils) (width 6 mils))
		(line (pt 200 mils -200 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 850 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "TPS7A2128PYWDJ" (originalName "TPS7A2128PYWDJ") (compHeader (numPins 4) (numParts 1) (refDesPrefix IC)
		)
		(compPin "A1" (pinName "IN") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "A2" (pinName "OUT") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B1" (pinName "EN") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "B2" (pinName "GND") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "TPS7A2128PYWDJ"))
		(attachedPattern (patternNum 1) (patternName "TPS7A2128PYWDJ")
			(numPads 4)
			(padPinMap
				(padNum 1) (compPinRef "A1")
				(padNum 2) (compPinRef "A2")
				(padNum 3) (compPinRef "B1")
				(padNum 4) (compPinRef "B2")
			)
		)
		(attr "Manufacturer_Name" "Texas Instruments")
		(attr "Manufacturer_Part_Number" "TPS7A2128PYWDJ")
		(attr "Mouser Part Number" "595-TPS7A2128PYWDJ")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Texas-Instruments/TPS7A2128PYWDJ?qs=MyNHzdoqoQJEpj%252BgRSc%252BRw%3D%3D")
		(attr "Arrow Part Number" "")
		(attr "Arrow Price/Stock" "")
		(attr "Mouser Testing Part Number" "")
		(attr "Mouser Testing Price/Stock" "")
		(attr "Description" "TPS7A21 500-mA, Low-Noise, Low-IQ, High-PSRR LDO")
		(attr "Datasheet Link" "https://www.ti.com/lit/ds/symlink/tps7a21.pdf?ts=1647212175126")
		(attr "Height" "0.3 mm")
	)

)
