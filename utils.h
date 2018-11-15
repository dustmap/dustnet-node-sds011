/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

#define DEVEUI_OFFSET 1
#define DEVEUI_LENGTH 8
#define APPKEY_OFFSET DEVEUI_OFFSET + DEVEUI_LENGTH
#define APPKEY_LENGTH 16


 // Remove Serial.print/Serial.println statements in production
 // see https://arduino.stackexchange.com/a/9858
 #ifdef ENABLE_DEBUG
	 #define Sprint(a) (Serial.print(a))
	 #define Sprintln(a) (Serial.println(a))
 #else
	 #define Sprint(a)
	 #define Sprintln(a)
 #endif

// Find the median in an array of integer
int median(int n, int samples[]) {
    for(int i=0; i<n-1; i++) {
        for(int j=i+1; j<n; j++) {
            if(samples[j] < samples[i]) {
                int temp = samples[i];
                samples[i] = samples[j];
                samples[j] = temp;
            }
        }
    }

    if(n%2 == 0) {
        return (int)((samples[n/2] + samples[n/2 - 1]) / 2);
    } else {
        return samples[n/2];
    }
}
