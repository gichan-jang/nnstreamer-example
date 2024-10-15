/**
 * @file	example_early_exit_capi.c
 * @date	16 Dec 2020
 * @brief	Early exit network example with multiple module using nnstreamer C-API
 * @author	Gichan Jang <gichan2.jnag@samsung.com>
 * @bug		No known bugs.
 */

#include <unistd.h>
#include <glib.h>
#include <nnstreamer.h>

/**
 * @brief callback for tensor sink.
 */
void
_sink_cb (const ml_tensors_data_h data,
    const ml_tensors_info_h info, void *user_data)
{
  g_critical ("[DEBUG] Touch sink callback.");
}

/**
 * @brief Main function.
 */
int
main (int argc, char **argv)
{
  gchar *str_pipeline;
  ml_pipeline_h pipe = NULL;
  ml_pipeline_sink_h sink_h = NULL;
  gchar *cam_src = g_strdup ("v4l2src");
  int ret = ML_ERROR_NONE;

  if (argv[1]) {
    g_free (cam_src);
    cam_src = g_strdup (argv[1]);
  }

  str_pipeline =
      g_strdup_printf
      ("%s ! videoscale ! videoconvert ! video/x-raw,format=RGB,width=640,height=480,framerate=30/1 ! tensor_converter ! tensor_sink name=sinkx", cam_src);
  g_critical ("pipeline description: %s", str_pipeline);

  ret = ml_pipeline_construct (str_pipeline, NULL, NULL, &pipe);
  if (ret != ML_ERROR_NONE) {
    g_critical ("Failed to construct the pipeline, ret: %d", ret);
    goto done;
  }

  ret = ml_pipeline_sink_register(pipe, "sinkx", _sink_cb, NULL, &sink_h);
  if (ret != ML_ERROR_NONE) {
    g_critical ("Failed to register the sink, ret: %d", ret);
    goto done;
  }

  ret = ml_pipeline_start (pipe);
  if (ret != ML_ERROR_NONE) {
    g_critical ("Failed to start the pipeline, ret: %d", ret);
    goto done;
  }

  sleep (10);

done:
  ml_pipeline_stop (pipe);
  ml_pipeline_sink_unregister (sink_h);
  ml_pipeline_destroy (pipe);

  g_free (str_pipeline);
  g_free (cam_src);

  return 0;
}
