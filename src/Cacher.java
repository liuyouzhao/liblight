import java.io.File;
import java.util.HashMap;

/**
 * For file cache
 */
public class Cacher {

    private boolean activated = true;

    /**
     * Data structure for constant order lookup of cache items.
     * Key: URL of page/image requested.
     * Value: File in storage associated with this key.
     */
    HashMap<String, File> cache;

    public Cacher() {
        cache = new HashMap<>();
    }

    public void enable() {
        activated = true;
    }

    public void disable() {
        activated = false;
    }

    public boolean isCachedFile(String urlString) {

        if(!activated) {
            return false;
        }

        File file = getCachedPage(urlString);
        if(file != null){
            System.out.println("Cached Copy found for : " + urlString + "\n");
            return true;
        }

        System.out.println("HTTP GET for : " + urlString + "\n");
        return false;
    }

    /**
     * Looks for File in cache
     * @param url of requested file
     * @return File if file is cached, null otherwise
     */
    public File getCachedPage(String url){
        return cache.get(url);
    }


    /**
     * Adds a new page to the cache
     * @param urlString URL of webpage to cache
     * @param fileToCache File Object pointing to File put in cache
     */
    public void addCachedPage(String urlString, File fileToCache){
        cache.put(urlString, fileToCache);
    }

    public void cacheFile() {

    }
}
