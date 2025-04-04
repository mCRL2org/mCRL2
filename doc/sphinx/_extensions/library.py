import subprocess

from sphinx.util import logging

_LOG = logging.getLogger(__name__)

def call(name, cmdline, stdin=None, return_err=False):
    """Runs cmdline and pipes stdin to the process' stdin. The standard output of
    the process is returned as a string. Any errors and warnings will be logged
    using the name provided. Raises a RuntimeError if the return code of the
    process was nonzero."""
    _LOG.debug('Running "{0}"'.format(" ".join(cmdline)))
    proc = subprocess.Popen(
        cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    out, err = proc.communicate(stdin)

    if proc.returncode:
        _LOG.error("{0} returned with non-zero status.".format(name))
        _LOG.error("{0} output: {1}".format(name, out.strip()))
        _LOG.error("{0} error output: {1}".format(name, err.strip()))
        raise RuntimeError("{0} returned with non-zero status.".format(name))

    if err and not return_err:
        _LOG.warning("{0} error output: {1}".format(name, err.strip()))

    if return_err:
        return (out, err)

    return out
