mofcomp: fbtusb.bmf

fbtusb.bmf: fbtusb.mof
        mofcomp -B:fbtusb.bmf fbtusb.mof
        wmimofck fbtusb.bmf





