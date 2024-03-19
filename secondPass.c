/*
 Created by OMER on 3/19/2024.
*/


#include "secondPass.h"

/*
 * iterate through symbol table: {
 *    if (data){
 *        update address with += IC
 *    }
 *    if (entry){
 *        if (unresolved){
 *            error
 *        } //else
 *        else if wasn't error till now {
 *            add to entry list
 *        }
 * }
 *
 * iterate through op list: {
 *    if there is unresolved symbol use{
 *        error
 *    }
 *    else if wasn't error till now {
 *        write in op file
 *        if (extern use){
 *            write in extern usages
 *        }
 *    }
 * }
 */

exit_code secondPass (file_analyze *f)
{

}
