/**
 * @file Sequence.h
 * @author Devon Crawford
 * @date February 14, 2019
 * @brief File containing the definitions and usage of the Sequence API:
 * A sequence is a list of clips in a realtime video editor
 */

#ifndef _SEQUENCE_API_
#define _SEQUENCE_API_

#include "Clip.h"
#include "LinkedListAPI.h"

/**
 * Define the Sequence structure.
 * A Sequence is a list of clips in a realtime video editor
 */
typedef struct Sequence {
    /*
        LinkedList of Clips in order of clip->pts
        Data type: struct Clip
     */
    List clips;
    /*
        ListIterator object used for iterating and seeking clips
     */
    ListIterator clips_iter;
    /*
        This is the fundamental unit of time (in seconds) in terms of which frame timestamps are represented.
     */
    AVRational time_base;
    /*
        Video frames per second.
     */
    double fps;
    /*
        Duration of a single video frame in time_base units.
        Calculation = time_base.den / fps
     */
    int video_frame_duration;

    /*
        Current location of the seek pointer within the sequence
        (We track this by video packets seen and seek usage)
     */
    int64_t current_frame_idx;

    /*
        Current clip index
     */
    int current_clip_idx;
} Sequence;

/**
 * Initialize new sequence and list of clips
 * @param  sequence Sequence is assumed to already be allocated memory
 * @return          >= 0 on success
 */
int init_sequence(Sequence *sequence, AVRational time_base, double fps);

/**
 * Insert Clip in sequence in sorted clip->pts order
 * @param  sequence Sequence containing a list of clips
 * @param  clip     Clip to be added into the sequence list of clips
 * @param  start_frame_index
 * @return          >= 0 on success
 */
void sequence_add_clip(Sequence *seq, Clip *clip, int start_frame_index);

/**
 * Insert Clip in sequence in sorted clip->pts order
 * @param  sequence Sequence containing a list of clips
 * @param  clip     Clip to be added into the sequence list of clips
 * @param  start_pts
 * @return          >= 0 on success
 */
void sequence_add_clip_pts(Sequence *seq, Clip *clip, int start_pts);

/**
 * Add clip to the end of sequence
 * @param seq  Sequence to insert clip
 * @param clip Clip to be inserted into end of sequence
 */
void sequence_append_clip(Sequence *seq, Clip *clip);

/**
 * Convert sequence frame index to pts (presentation time stamp)
 * @param  seq         Sequence
 * @param  frame_index index of frame to convert
 * @return             pts representation of frame in sequence
 */
int64_t seq_frame_index_to_pts(Sequence *seq, int frame_index);

/**
 * Convert pts to frame index in sequence
 * @param  seq Sequence
 * @param  pts presentation time stamp to convert into frame index
 * @return     presentation time stamp representation of frame index
 */
int seq_pts_to_frame_index(Sequence *seq, int64_t pts);

/**
 * Determine if sequence frame lies within a clip (assuming clip is within sequence)
 * Example:
 * If this[xxx] is a clip where |---| is VideoContext: |---[XXXXX]-----|
 * then return of 0 would be the first X and return of 1 would be the second X and so on..
 * We can use the successful return of this function with seek_clip_pts() to seek within the clip!!
 * @param  sequence    Sequence containing clip
 * @param  clip        Clip within sequence
 * @param  frame_index index in sequence
 * @return             on success: pts relative to clip, and clip timebase (where zero represents clip->orig_start_pts)
 *                     on fail: < 0
 */
int64_t seq_frame_within_clip(Sequence *seq, Clip *clip, int frame_index);

/**
 * Seek to an exact frame within the sequence (and all the clips within it)!
 * @param  seq         Sequence containing clips
 * @param  frame_index index in sequence, will be used to find clip and the clip frame
 * @return             >= 0 on success
 */
int sequence_seek(Sequence *seq, int frame_index);

/**
 * Read our editing sequence!
 * This will iterate our clips wherever sequence_seek() left off
 * This function uses clip_read_packet() and av_read_frame() internally. This function
 * works the exact same.. reading one packet at a time and incrementing internally.
 * (call this function in a loop while >= 0 to get full edit)
 * @param  seq Sequence containing clips
 * @param  pkt output AVPacket
 * @return     >= 0 on success, < 0 when reached end of sequence or error.
 */
int sequence_read_packet(Sequence *seq, AVPacket *pkt);

/**
 * Sets the start_pts of a clip in sequence
 * @param  seq               Sequence containing clip
 * @param  clip              Clip to set start_pts
 * @param  start_frame_index frame index in sequence to start the clip
 * @return                   >= 0 on success
 */
int move_clip(Sequence *seq, Clip *clip, int start_frame_index);

/**
 * Sets the start_pts of a clip in sequence
 * @param  seq               Sequence containing clip
 * @param  clip              Clip to set start_pts
 * @param  start_pts         pts in sequence to start the clip
 * @return                   >= 0 on success
 */
void move_clip_pts(Sequence *seq, Clip *clip, int64_t start_pts);

/**
 * Get current clip from sequence (seek position for next read)
 * @param  seq Sequence containing clips
 * @return     Clip that is currently being read
 */
Clip *get_current_clip(Sequence *seq);

/**
 * Free entire sequence and all clips within
 * @param seq Sequence containing clips and clip data to be freed
 */
void free_sequence(Sequence *seq);

/*************** EXAMPLE FUNCTIONS ***************/
/**
 * Test example showing how to read packets from sequence
 * @param seq Sequence to read
 */
void example_sequence_read_packets(Sequence *seq);

#endif