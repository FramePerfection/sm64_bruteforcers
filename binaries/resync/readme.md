# Resync Module
The resync module attempts to recreate a given reference run by finding the best input for each frame to "match" the respective frame in the reference run.
What "matching" entails is described in further detail [below](#Matching).

##  Getting started
### Running the demo
Running `main.exe` with the default configuration will match a short section of the WIP Super Mario 74 TAS, starting from a slightly offset starting position.  
It will load in `empty-frames.m64` (containing frames with no input), do its best to match every frame as best as possible, and output a file called `synced.64`.
You can exit the program once it prints `Matching frames done!`.

You can then run this .m64 file in mupen; the savestate `synced.st` already has the ghost hack enabled, so you can load in `original.ghost` in STROOP and see the comparison - 
the difference should be very minute (as it had been from the start).  

The run involves an SBLJ, a C-Up slide, and two dive grinds (one on flat ground and one on a slope).

### Resyncing a different run
Follow these steps to resync a run of your choice:
1. Create a reference run:  
Open you .m64-file in mupen and load a savestate from which you'd like to resync.
Then, run the `record_reference.lua` script in mupen, and let your movie continue until a little further than the point you want to resync to.
2. Open the run you want to sync to the reference in mupen and find the frame at which you want to begin to resync. Set a savestate there.
3. Open the resync module in STROOP and load the .m64 file you want to sync to the reference.
4. Set the `m64_start` value to the frame number at the state you saved before (plus one if you're using mupen's 0-indexing).
5. Set the `m64_end` to the frame at which you want to stop resyncing.
6. Optionally adjust the weights. See [matching](#Matching) for details.
7. Hit "Apply state".
8. Hit "Run".

The point at which you wish to start resyncing should be some frames after the start of the reference run -
the option `auto_detect_reference_offset` will find the appropriate offset and start resyncing from there.  
The bruteforcer will output the error for every frame, that is, the mismatch between what its best found input produces and the reference frame, based on the assigned weights. (Greedy syncing)
Large jumps in error between frames may indicate that the bruteforcer failed to find a suitable input before failing completely.  
When the bruteforcer cannot find an input that yields the same action as the reference run on a frame,
it will print this event and then start optimizing the last `num_fine_tuning_frames` for minimal overall error (sum of least squares),
in hopes of matching at least one more reference frame in action. If it can manage this, the bruteforcer will go back greedy syncing.  
Note that this approach is not very promising and subject to change, as most of the time the bruteforcer is already stuck when this happens.
Instead, you may want to resync shorter segments with different weights for now, or attempt to get a better initial match manually.  
Note that the weight configuration can have a drastic effect on how successful a resync attempt is, so experiment with them to see if you can get better results "for free".

## Matching
On every frame, the bruteforcer calculates an error term, that is, the difference between the reference frame and the state a new input yields.  
The error term is calculated as  
```(dx * x_weight)^2 + (dy * y_weight)^2 + (dz * z_weight)^2 + (dHSpeed * angle_weight)^2 + (dFaceAngle * angle_weight)^2``` where 
`dx`, `dy`, and `dz` are the differences in their respective `gMarioState->pos`,  
`dHSpeed` is the difference in `gMarioState->forwardVel`,  
`dFaceAngle` is the difference in `gMarioState->faceAngle[1]` and  
`..._weight` is the respective weight in the bruteforcer configuration.

As such, higher weights will yield higher error terms. This does not indicate a larger mismatch between frames, however.  
Also, due to its nature as an angle value, the `angle_weight` should be significantly smaller than the x-, y- and z-weights, usually around 0.05.
In a similar vein, matching horizontal speed is usually more important than the immediate position, because it effects many frames beyond the immediate next frame - 
therefore, it should be set significantly higher than the x-, y- and z-weights, something between 2 and 10 may give decent results (and vary quite drastically).