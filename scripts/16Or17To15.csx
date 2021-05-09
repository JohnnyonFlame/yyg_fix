////Downgrade from bytecode 16 or 17 to 15 - by Grossley

EnsureDataLoaded();

string oldByte = Data.GeneralInfo.BytecodeVersion.ToString();

if (Data?.GeneralInfo.BytecodeVersion >= 16)
{
    if (!ScriptQuestion("Downgrade bytecode from " + oldByte +" to 15?"))
    {
        ScriptMessage("Cancelled.");
        return;
    }
    if (Data?.GeneralInfo.BytecodeVersion == 17)
	{
		//TODO - test this function
		//Data.GMS2_2_2_302 = false;
	    if (Data.FORM.Chunks.ContainsKey("TGIN"))
	        Data.FORM.Chunks.Remove("TGIN");
	}

    if (Data.FORM.Chunks.ContainsKey("LANG"))
        Data.FORM.Chunks.Remove("LANG");
    if (Data.FORM.Chunks.ContainsKey("GLOB"))
        Data.FORM.Chunks.Remove("GLOB");
    if (Data.FORM.Chunks.ContainsKey("EMBI"))
        Data.FORM.Chunks.Remove("EMBI");
    
	Data.Options.Constants.Clear();

	for (int i = 0; i < Data.Strings.Count; i++)
	{
		if (Data.Strings[i].Content == "@@SleepMargin")
			Data.Strings.Remove(Data.Strings[i]);
		if (Data.Strings[i].Content == "@@DrawColour")
			Data.Strings.Remove(Data.Strings[i]);
		if (Data.Strings[i].Content == "4294967295")
			Data.Strings.Remove(Data.Strings[i]);
	}

    Data.GeneralInfo.BytecodeVersion = 15;
    Data.GeneralInfo.Major = 1;
    Data.GeneralInfo.Build = 1804;
    ScriptMessage("Downgraded from " + oldByte + " to 15 successfully. Save the game to apply the changes.");
}
else
{
    ScriptError("Not bytecode 16 or 17.", "Error");
    return;
}

/*
string x = "String[] order = {\"";
foreach (string key in Data.FORM.Chunks.Keys)  
{
    if (key != "AUDO")
        x += (key + "\", \"");
    else
        x += (key + "\"};");
}
return x;
*/