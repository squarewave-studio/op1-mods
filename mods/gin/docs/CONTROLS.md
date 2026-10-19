# Controls And Saving

This guide describes GIN 1.0's standalone original-OP-1 #246 build.

## Play

Press a drum key to select and play its sound. Each kit fills all 24 keys.

| Encoder | Turn | Shift + Turn |
| --- | --- | --- |
| Blue | Sound morph, A to B | Sound morph, A to B |
| Green | Tone pitch | Pitch modulation amount |
| White | Tone decay | Noise decay |
| Orange | Tone/noise balance | Noise filter cutoff |

Click Blue to choose which endpoint to edit. Moving morph towards A or B
selects that endpoint; at the exact centre, the previous edit side is retained.
Tone and noise decay are separate parameters. Editing affects subsequent hits;
a hit that is already playing keeps the settings it started with.

## Advanced

Press T1 to enter Advanced. Use T2/T3 or the left and right arrows to page through Tone,
Tone Envelope, Pitch Mod, Noise Filter, Noise Envelope, Mix And Level, EQ and
Velocity. About is the final page. Press T1 again to return to Play.

The coloured labels identify the controls on each page. Blue remains morph.
Use the endpoint indicator to check whether an edit changes A or B.

## Sound Names

Press T4 (Name) on an Advanced parameter page. Blue or the arrows move the cursor,
Green chooses a character and Orange selects insertion or deletion. T1 applies the name and T4 cancels. Names can contain up to 23 characters.
Cancelling leaves the applied name unchanged.

## Working Slots And Named Snapshots

Applied sound names, full A/B parameters and morph are saved to the current
working slot. They can survive slot changes and restart without creating a
new named preset. Cancelled name drafts are not saved.

Hold a 1–8 snapshot button until completion to create the separate named
snapshot. Reloading it restores the saved kit state. An ordinary encoder edit
does not create or overwrite a named preset.

If SAVE RETRY appears, the edit remains pending. Wait for the save to complete
before switching off. GIN does not claim power-loss-proof storage.
